/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkGeodesicActiveContoursTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkFastMarchingImageFilter.h"
#include "itkGeodesicActiveContourImageFilter.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkCommand.h"

namespace
{
// The following classe is used to support callbacks
// on the filter in the pipeline that follows later
class ShowProgressObject
{
public:
  ShowProgressObject(itk::ProcessObject* o)
    {m_Process = o;}
  void ShowProgress()
    {std::cout << "Progress " << m_Process->GetProgress() << std::endl;}
  itk::ProcessObject::Pointer m_Process;
};

}


int itkGeodesicActiveContoursTest(int, char**)
{

  /* -------------------------------------------------
   * Create an initial level set of dimension 64x64
   * with the zero set being a circle of radius 8
   * centered at (28,35).
   */

  // create a fastmarching object
  typedef itk::Image<float,2> FloatImage;
  typedef itk::FastMarchingImageFilter<FloatImage> FastMarcherType;

  FastMarcherType::Pointer marcher = FastMarcherType::New();
  
  // setup trial points
  typedef FastMarcherType::NodeType NodeType;
  typedef FastMarcherType::NodeContainer NodeContainer;

  NodeContainer::Pointer trialPoints = NodeContainer::New();

  NodeType node;

  FloatImage::IndexType index0 = {{28,35}};
  
  node.SetValue( 0.0 );
  node.SetIndex( index0 );
  trialPoints->InsertElement(0, node);
  
  marcher->SetTrialPoints( trialPoints );

  // specify the size of the output image
  FloatImage::SizeType size = {{64,64}};
  marcher->SetOutputSize( size );

  // update the marcher
  marcher->Update();

  // walk the marcher output
  FloatImage::Pointer levelSet = marcher->GetOutput();
  itk::ImageRegionIterator<FloatImage>
    iterator( levelSet, levelSet->GetBufferedRegion() );

  for( ; !iterator.IsAtEnd(); ++iterator )
    {
    iterator.Set( iterator.Get() - 8.0 );
    }


  /* -------------------------------------------------
   * Create a edge potential image with values of
   * one
   */
  FloatImage::Pointer edgeImg = FloatImage::New();
  edgeImg->CopyInformation( levelSet );
  edgeImg->SetBufferedRegion(
    levelSet->GetBufferedRegion() );
  edgeImg->Allocate();

  itk::ImageRegionIterator<FloatImage>
    edgeIter( edgeImg, edgeImg->GetBufferedRegion() );
  
  for( ; !edgeIter.IsAtEnd(); ++edgeIter )
    {
    edgeIter.Set( 1.0 );
    }

  /* -----------------------------------------------
   * Create a edge potential derivative image with
   * all values 0  - use for both dimensions
   */
  FloatImage::Pointer derivImg = FloatImage::New();
  derivImg->CopyInformation( levelSet );
  derivImg->SetBufferedRegion(
    levelSet->GetBufferedRegion() );
  derivImg->Allocate();

  itk::ImageRegionIterator<FloatImage>
    derivIter( derivImg, derivImg->GetBufferedRegion() );

  for( ; !derivIter.IsAtEnd(); ++derivIter )
    {
    derivIter.Set( 0.0 );
    }

  /* -----------------------------------------------
   * Create a geodesic active contour object
   * and test the full-band version of the algorithm
   */

  typedef itk::GeodesicActiveContourImageFilter<FloatImage,FloatImage,FloatImage>
    ShapeDetectorType;

  ShapeDetectorType::Pointer detector = ShapeDetectorType::New();

  // create progress observer
  ShowProgressObject progressWatch( detector );
  itk::SimpleMemberCommand<ShowProgressObject>::Pointer command;
  command = itk::SimpleMemberCommand<ShowProgressObject>::New();
  command->SetCallbackFunction(&progressWatch,
                               &ShowProgressObject::ShowProgress);
  detector->AddObserver( itk::ProgressEvent(), command);


  detector->SetInput( levelSet );
  detector->SetEdgeImage( edgeImg );
  detector->SetDerivativeImage( derivImg, 0 );
  detector->SetDerivativeImage( derivImg, 1 );
  detector->SetPropagateOutwards( true );
  detector->SetInflationStrength( 0.5 );

  std::cout << "Running full-band version" << std::endl;
  detector->Update();

  /* --------------------------------------------------
   * Test the narrow-band version of the algorithm
   */
  detector->NarrowBandingOn();
  detector->SetNarrowBandwidth( 10.0 );
  detector->SetNumberOfIterations( 10 );

  std::cout << "Running narrow-band version" << std::endl;
  detector->Update();
  
  // Exercise various member variables
  detector->Print( std::cout );

  /* ---------------------------------------------------
   * test some componentes
   */
  typedef itk::UpwindDerivativeImageFunction<FloatImage>
    FunctionType;
  
  FunctionType::Pointer func = FunctionType::New();
  
  typedef FunctionType::IndexType FunctionIndexType;
  typedef FunctionType::ContinuousIndexType FunctionContinuousIndexType;

  FunctionIndexType index;
  index.Fill( 0 );
  
  FunctionContinuousIndexType contIndex;
  contIndex.Fill( 0.0 );
  
  func->SetInputImage( levelSet );
  func->EvaluateAtContinuousIndex( contIndex );
  func->EvaluateAtIndex( index );
  func->EvaluateNthDerivativeAtIndex( index, 1 );

  return EXIT_SUCCESS;

}
