#include <stdio.h>

// Native ITK stuff
#include "itkSize.h"
#include "itkIndex.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkPoint.h"

// Blox stuff
#include "itkBloxBoundaryProfileImage.h"
#include "itkBloxBoundaryPointPixel.h"
#include "itkBloxBoundaryPointImage.h"
#include "itkGradientImageToBloxBoundaryPointImageFilter.h"
#include "itkBloxBoundaryPointImageToBloxBoundaryProfileImageFilter.h"

// Spatial function stuff
#include "itkSphereSpatialFunction.h"
#include "itkFloodFilledSpatialFunctionConditionalIterator.h"

// DOG gradient related stuff
#include "itkBinomialBlurImageFilter.h"
#include "itkDifferenceOfGaussiansGradientImageFilter.h"

#include "itkExceptionObject.h"


int main(int, char**)
{
  const unsigned int dim = 3;
  const unsigned int size = 20;

  //-----------------Create a new input image--------------------

  // Image size and spacing parameters
  unsigned long sourceImageSize[]  = { size,size,size };
  double sourceImageSpacing[] = { 1.0,1.0,1.0 };
  double sourceImageOrigin[] = { 0,0,0 };

  // Image typedef
  typedef itk::Image< unsigned char, dim > TImageType;

  // Creates the sourceImage (but doesn't set the size or allocate memory)
  TImageType::Pointer sourceImage = TImageType::New();
  sourceImage->SetOrigin(sourceImageOrigin);
  sourceImage->SetSpacing(sourceImageSpacing);

  printf("New sourceImage created\n");

  // The following block allocates the sourceImage

  // Create a size object native to the sourceImage type
  TImageType::SizeType sourceImageSizeObject;
  // Set the size object to the array defined earlier
  sourceImageSizeObject.SetSize( sourceImageSize );
  // Create a region object native to the sourceImage type
  TImageType::RegionType largestPossibleRegion;
  // Resize the region
  largestPossibleRegion.SetSize( sourceImageSizeObject );
  // Set the largest legal region size (i.e. the size of the whole sourceImage) to what we just defined
  sourceImage->SetLargestPossibleRegion( largestPossibleRegion );
  // Set the buffered region
  sourceImage->SetBufferedRegion( largestPossibleRegion );
  // Set the requested region
  sourceImage->SetRequestedRegion( largestPossibleRegion );
  // Now allocate memory for the sourceImage
  sourceImage->Allocate();

  printf("New sourceImage allocated\n");

  // Initialize the image to voxel values of 32
  itk::ImageRegionIterator<TImageType> it = 
    itk::ImageRegionIterator<TImageType>(sourceImage, largestPossibleRegion);

  for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
    it.Set(32);
    }

  //---------Put a sphere in the input image-----------

  typedef itk::SphereSpatialFunction<dim> TFunctionType;
  typedef TFunctionType::InputType TFunctionPositionType;

  // Create and initialize a new sphere function
  TFunctionType::Pointer spatialFunc = TFunctionType::New();
  spatialFunc->SetRadius( 7 );

  // Set center of spatial function to (10,10,10)
  TFunctionPositionType center;
  center[0]=10;
  center[1]=10;
  center[2]=10;
  spatialFunc->SetCenter(center);

  printf("Sphere spatial function created\n");

  // Create and initialize a spatial function iterator
  TImageType::IndexType seedPos;
  const TImageType::IndexValueType pos[] = {10,10,10};
  seedPos.SetIndex(pos);

  typedef itk::FloodFilledSpatialFunctionConditionalIterator
    <TImageType, TFunctionType> TItType;
  TItType sfi = TItType(sourceImage, spatialFunc, seedPos);

  // Iterate through the entire image and set interior pixels to 255
  for( ; !( sfi.IsAtEnd() ); ++sfi)
    {
    sfi.Set(255);
    }

  printf("Spatial function iterator created, sphere drawn\n");

  //--------------------Do blurring and edge detection----------------

  typedef TImageType TOutputType;
  
  // Create a binomial blur filter
  itk::BinomialBlurImageFilter<TImageType, TOutputType>::Pointer binfilter;
  binfilter = itk::BinomialBlurImageFilter<TImageType, TOutputType>::New();

  sourceImage->SetRequestedRegion(sourceImage->GetLargestPossibleRegion() );

  // Set filter parameters
  binfilter->SetInput(sourceImage);
  binfilter->SetRepetitions(1);

  // Set up the output of the filter
  TImageType::Pointer blurredImage = binfilter->GetOutput();

  // Execute the filter
  binfilter->Update();
  std::cout << "Binomial blur filter updated\n";

  // Create a differennce of gaussians gradient filter
  typedef itk::DifferenceOfGaussiansGradientImageFilter<TOutputType,
    double> TDOGFilterType;
  TDOGFilterType::Pointer DOGFilter = TDOGFilterType::New();

  // We're filtering the output of the binomial filter
  DOGFilter->SetInput(blurredImage);

  // Get the output of the gradient filter
  TDOGFilterType::TOutputImage::Pointer gradientImage = DOGFilter->GetOutput();

  // Go!
  DOGFilter->Update();

  //------------------------Blox Boundary Point Analysis-------------------------

  typedef itk::GradientImageToBloxBoundaryPointImageFilter<TDOGFilterType::TOutputImage> TBPFilter;
  typedef TBPFilter::TOutputImage TBloxBPImageType;

  TBPFilter::Pointer bpFilter= TBPFilter::New();
  bpFilter->SetInput( DOGFilter->GetOutput() );

  TBloxBPImageType::Pointer bloxBoundaryPointImage = bpFilter->GetOutput();

  bpFilter->Update();

  //------------------------Blox Profile Analysis---------------------------------

  typedef itk::itkBloxBoundaryPointImageToBloxBoundaryProfileImageFilter< TImageType > TProfileFilter;
  typedef itk::BloxBoundaryProfileImage< dim > TBloxProfileImageType;

  TProfileFilter::Pointer profileFilter = TProfileFilter::New();
  std::cerr << "profile filter created" << std::endl;

  profileFilter->SetInput1( blurredImage );
  profileFilter->SetInput2( bloxBoundaryPointImage );
  std::cerr << "input images set" << std::endl;

  double setUniqueAxis = 5;
  double setSymmetricAxes = 2;
  unsigned int numberOfBins = 5;
  unsigned int splatMethod = 0;
  unsigned int spaceDimension = 4;

  profileFilter->Initialize(setUniqueAxis, setSymmetricAxes, numberOfBins, 
    splatMethod, spaceDimension);
  std::cerr << "profile filter initialized" << std::endl;

  TBloxProfileImageType::Pointer bloxBoundaryProfileImage = profileFilter->GetOutput();

  try{profileFilter->Update();}
  catch( itk::ExceptionObject  & myException )
    {
    std::cerr << "Exception caught in Update() method" << std::endl;
    std::cerr << myException << std::endl;
    return EXIT_FAILURE;
    }

  //verification

  double rad = spatialFunc->GetRadius();
  bool passed = true;

    //-------------------Pull boundary profiles out of the image----------------------

  // Create an iterator that will walk the blox image
  typedef itk::ImageRegionIterator<TBloxProfileImageType> BloxIterator;

  //profile iterator
  BloxIterator bloxIt = BloxIterator(bloxBoundaryProfileImage,
                                     bloxBoundaryProfileImage->GetRequestedRegion() );

  // Used for obtaining the index of a pixel
  TBloxProfileImageType::IndexType bloxindex;

  // Used for obtaining position data from a BloxPoint
  itk::Point<double, 3> position;

  // Position are we at in the list
  int depth = 0;

  for ( bloxIt.GoToBegin(); !bloxIt.IsAtEnd(); ++bloxIt)
    {
    // The iterator for accessing linked list info from profile pixel
    itk::BloxBoundaryProfilePixel<3>::iterator bpiterator;

    // What position are we at in the list?
    depth = 0;

    // Get the index of the pixel
    bloxindex = bloxIt.GetIndex();

    // Walk through all of the elements at the pixel
    for (bpiterator = bloxIt.Value().begin(); bpiterator != bloxIt.Value().end(); ++bpiterator)
      {
      position = (*bpiterator)->GetOptimalBoundaryLocation();
      depth++;
      
      std::cout << "Boundary profile at ";
      std::cout << "Position=(" << position[0] << " " << position[1] << " " << position[2] << ") ";
      std::cout << "at index=(" << bloxindex.m_Index[0] << " " << bloxindex.m_Index[1] << " " << bloxindex.m_Index[2] << "),";
      std::cout << "depth=" << depth << "\n";
      } // end iterate
    }
  return EXIT_SUCCESS;
}
