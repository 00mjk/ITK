/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBloxBoundaryPointImageTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <stdio.h>

// Native ITK stuff
#include "itkSize.h"
#include "itkIndex.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkPoint.h"

// Blox stuff
#include "itkBloxBoundaryPointPixel.h"
#include "itkBloxBoundaryPointImage.h"

// Spatial function stuff
#include "itkSphereSpatialFunction.h"
#include "itkFloodFilledSpatialFunctionConditionalIterator.h"

// DOG gradient related stuff
#include "itkBinomialBlurImageFilter.h"
#include "itkDifferenceOfGaussiansGradientImageFilter.h"

// Main for testing BloxImage/BloxPixel storage
int itkBloxBoundaryPointImageTest(int, char**)
{
  const unsigned int dim = 3;

  //-----------------Create a new input image--------------------
  // Image size and spacing parameters
  unsigned long sourceImageSize[]  = { 20,20,20 };
  double sourceImageSpacing[] = { 1.0,1.0,1.0 };
  double sourceImageOrigin[] = { 0,0,0 };

  // Image typedef
  typedef itk::Image< unsigned char, dim > TImageType;

  // Creates the sourceImage (but doesn't set the size or allocate memory)
  TImageType::Pointer sourceImage = TImageType::New();
  sourceImage->SetOrigin(sourceImageOrigin);
  sourceImage->SetSpacing(sourceImageSpacing);

  printf("New sourceImage created\n");

  //-----The following block allocates the sourceImage-----

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

  // Initialize the image to hold all 0's
  itk::ImageRegionIterator<TImageType> it = 
    itk::ImageRegionIterator<TImageType>(sourceImage, largestPossibleRegion);

  for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
    it.Set(0);
    }

  //---------Create and initialize a spatial function-----------

  typedef itk::SphereSpatialFunction<dim> TFunctionType;
  typedef TFunctionType::InputType TFunctionPositionType;

  // Create and initialize a new sphere function

  TFunctionType::Pointer spatialFunc = TFunctionType::New();
  spatialFunc->SetRadius( 5 );

  TFunctionPositionType center;
  center[0]=10;
  center[1]=10;
  center[2]=10;
  spatialFunc->SetCenter(center);

  printf("Sphere spatial function created\n");

  //---------Create and initialize a spatial function iterator-----------
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
  binfilter->SetRepetitions(4);

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

  // Image size and spacing parameters
  unsigned long bloxImageSize[]  = {1,1,1};
  double bloxImageSpacing[] = {20,20,20};
  double bloxImageOrigin[] = { 0,0,0 };

  typedef itk::BloxBoundaryPointImage<TDOGFilterType::TOutputImage> TBloxImageType;

  // Creates the bloxPointImage (but doesn't set the size or allocate memory)
  TBloxImageType::Pointer bloxBoundaryPointImage = TBloxImageType::New();
  bloxBoundaryPointImage->SetOrigin(bloxImageOrigin);
  bloxBoundaryPointImage->SetSpacing(bloxImageSpacing);

  // Create a size object native to the TBloxImageType bloxBoundaryPointImage type
  TBloxImageType::SizeType size = {{0}};

  // Set the size object to the array defined earlier
  size.SetSize( bloxImageSize );

  // Create a region object native to the TBloxImageType bloxBoundaryPointImage type
  TBloxImageType::RegionType theregion;

  // Resize the region
  theregion.SetSize( size );

  // Set the largest legal region size (i.e. the size of the whole bloxBoundaryPointImage) to what we just defined
  bloxBoundaryPointImage->SetLargestPossibleRegion( theregion );
  bloxBoundaryPointImage->SetBufferedRegion( theregion );
  bloxBoundaryPointImage->SetRequestedRegion( theregion );

  // Now allocate memory for the bloxBoundaryPointImage
  bloxBoundaryPointImage->Allocate();
  
  // Fill the BloxBoundaryPointImage with boundary points
  bloxBoundaryPointImage->SetThreshold(128);
  bloxBoundaryPointImage->SetSourceImage(gradientImage);
  bloxBoundaryPointImage->FindBoundaryPoints();

  //-------------------Pull boundary points out of the image----------------------

  // Create an iterator that will walk the blox image
  typedef itk::ImageRegionIterator<TBloxImageType> BloxIterator;

  BloxIterator bloxIt = BloxIterator(bloxBoundaryPointImage,
                                     bloxBoundaryPointImage->GetRequestedRegion() );

  // Used for obtaining the index of a pixel
  TBloxImageType::IndexType bloxindex;

  // Used for obtaining position data from a BloxPoint
  itk::Point<double, 3> position;
  
  // Position are we at in the list
  int depth = 0;

  for ( bloxIt.GoToBegin(); !bloxIt.IsAtEnd(); ++bloxIt)
    {
    // The iterator for accessing linked list info
    itk::BloxBoundaryPointPixel<3>::iterator bpiterator;

    // What position are we at in the list?
    depth = 0;

    // Get the index of the pixel
    bloxindex = bloxIt.GetIndex();

    // Walk through all of the elements at the pixel
    for (bpiterator = bloxIt.Value().begin(); bpiterator != bloxIt.Value().end(); ++bpiterator)
      {
      position = (*bpiterator)->GetPhysicalPosition();
      depth++;
      std::cout << "Boundary point at ";
      std::cout << "Position=(" << position[0] << " " << position[1] << " " << position[2] << ") ";
      std::cout << "at index=(" << bloxindex.m_Index[0] << " " << bloxindex.m_Index[1] << " " << bloxindex.m_Index[2] << "),";
      std::cout << "depth=" << depth << "\n";
      } // end iterate
    }

  return EXIT_SUCCESS;
}
