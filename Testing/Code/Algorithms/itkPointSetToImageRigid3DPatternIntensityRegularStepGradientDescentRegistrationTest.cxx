/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit (ITK)
  Module:    itkPointSetToImageRigid3DPatternIntensityRegularStepGradientDescentRegistrationTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkPointSetToImageRigid3DPatternIntensityRegularStepGradientDescentRegistration.h"

#include "itkCommandIterationUpdate.h"

/** 
 *  This test uses two 3D-Gaussians (standard deviation RegionSize/2)
 * 
 *  One of the images is subsampled in order to obtain a PoinSet.
 *  The PointSet is considered the Target and the other image is
 *  considered the Reference.
 */ 

int main()
{

  // Number of pixels to take from the target image
  unsigned long  numberOfSamples = 157L;

  /*Allocate Images*/
  const unsigned int ImageDimension  = 3;

  typedef itk::Image<unsigned char,ImageDimension>  ReferenceType;

  typedef itk::DefaultStaticMeshTraits<unsigned char,   // pixel type
                                       ImageDimension,  // point dimension
                                       1,               // max topological dim
                                       double,          // coordinates type
                                       double           // interpolation weight
                                         >  DefaultPointSetTraits;

  typedef itk::PointSet<unsigned char,ImageDimension,DefaultPointSetTraits>    PointSetType;

  typedef PointSetType                                          TargetType;

  typedef itk::PointSetToImageRigid3DPatternIntensityRegularStepGradientDescentRegistration<
                                                                ReferenceType,
                                                                TargetType> RegistrationType;


  typedef itk::CommandIterationUpdate<  RegistrationType::OptimizerType  >
                                                                 CommandIterationType;

  typedef RegistrationType::OptimizerType::TransformType          OptimizerParametersScaleTransformType;
  typedef OptimizerParametersScaleTransformType::ParametersType   ScaleTransformParametersType;


  ReferenceType::SizeType size = {{100,100,100}};
  ReferenceType::IndexType index = {{0,0,0}};
  ReferenceType::RegionType region;
  region.SetSize( size );
  region.SetIndex( index );

  ReferenceType::Pointer imgReference = ReferenceType::New();
  imgReference->SetLargestPossibleRegion( region );
  imgReference->SetBufferedRegion( region );
  imgReference->SetRequestedRegion( region );
  imgReference->Allocate();

  ReferenceType::Pointer imgTarget = ReferenceType::New(); // type=Reference but this 
                                                           // is the target image to subsample
  imgTarget->SetLargestPossibleRegion( region );
  imgTarget->SetBufferedRegion( region );
  imgTarget->SetRequestedRegion( region );
  imgTarget->Allocate();

  /* Fill images with a 3D gaussian*/
  typedef  itk::ImageRegionIteratorWithIndex<ReferenceType> ReferenceIteratorType;


  itk::Point<double,ImageDimension> center;
  center[0] = static_cast<double>( region.GetSize()[0] ) / 2.0;
  center[1] = static_cast<double>( region.GetSize()[1] ) / 2.0;
  center[2] = static_cast<double>( region.GetSize()[2] ) / 2.0;

  const double s = (double)region.GetSize()[0]/2.0;

  itk::Point<double,ImageDimension>  p;
  itk::Vector<double,ImageDimension> d;

  /* Set the displacement */
  itk::Vector<double,ImageDimension> displacement;
  displacement[0] = 7.0;
  displacement[1] = 3.0;
  displacement[2] = 3.0;

  ReferenceIteratorType ri(imgReference,region);
  ReferenceIteratorType ti(imgTarget,region);

  ri.GoToBegin();
  while(!ri.IsAtEnd())
  {
    p[0] = ri.GetIndex()[0];
    p[1] = ri.GetIndex()[1];
    p[2] = ri.GetIndex()[2];
    d = p-center;
    d += displacement;
    const double x = d[0];
    const double y = d[1];
    const double z = d[2];
    const double value = 200.0 * exp( - ( x*x + y*y + z*z )/(s*s) );
    ri.Set( static_cast<ReferenceType::PixelType>( value ) );
    ++ri;
  }

  ti.GoToBegin();
  while(!ti.IsAtEnd())
  {
    p[0] = ti.GetIndex()[0];
    p[1] = ti.GetIndex()[1];
    p[2] = ti.GetIndex()[2];
    d = p-center;
    const double x = d[0];
    const double y = d[1];
    const double z = d[2];
    const double value = 200.0 * exp( - ( x*x + y*y * z*z )/(s*s) );
    ti.Set( static_cast<TargetType::PixelType>( value ) ); 
    ++ti;
  }

  // Subsample the target image to produce a point set
  PointSetType::Pointer pointSetTarget = PointSetType::New();

  PointSetType::PointsContainer::Pointer    points = pointSetTarget->GetPoints();
  PointSetType::PointDataContainer::Pointer data   = PointSetType::PointDataContainer::New();
  pointSetTarget->SetPointData( data );

  points->Reserve( numberOfSamples );
  data->Reserve(   numberOfSamples );

  const unsigned int numPixelsToSkip =
            imgTarget->GetOffsetTable()[ImageDimension] / numberOfSamples;

  unsigned int counter   = 0;
  unsigned int numPoints = 0;


  ti.GoToBegin();
  while(!ti.IsAtEnd() && numPoints < numberOfSamples )
  {
    if( counter >= numPixelsToSkip ) 
    {
      PointSetType::PointType point;
      point[0] = ti.GetIndex()[0];
      point[1] = ti.GetIndex()[1];
      point[2] = ti.GetIndex()[2];
      points->SetElement( numPoints, point    );
      data->SetElement(   numPoints, ti.Get() );
      counter = 0;
      numPoints++;
    }
    ++ti;
    ++counter;
  }





  
  RegistrationType::Pointer registrationMethod = RegistrationType::New();

  CommandIterationType::Pointer iterationCommand = CommandIterationType::New();

  iterationCommand->SetOptimizer(  registrationMethod->GetOptimizer() );

  registrationMethod->GetOptimizer()->AddObserver( itk::Command::IterationEvent,
                                                   iterationCommand ); 

  ScaleTransformParametersType  parametersScale;
  parametersScale[0] = 1.00;
  parametersScale[1] = 1.00;
  parametersScale[2] = 1.00;
  parametersScale[3] = 0.01;
  parametersScale[4] = 0.01;
  parametersScale[5] = 0.01;
  

  
  registrationMethod->SetReference(imgReference);
  registrationMethod->SetTarget(pointSetTarget);

  registrationMethod->GetMetric()->SetLambda( 10.0 );

  RegistrationType::OptimizerType::Pointer optimizer = 
                                  registrationMethod->GetOptimizer();

  optimizer->GetTransform()->SetParameters( parametersScale );
  optimizer->SetMinimize();
  optimizer->SetGradientMagnitudeTolerance( 1e-6 );
  optimizer->SetMaximumStepLength( 0.01 );
  optimizer->SetMinimumStepLength( 1e-6 );
  optimizer->SetNumberOfIterations( 100 );


  registrationMethod->StartRegistration();


  // get the results
  RegistrationType::ParametersType solution = 
    registrationMethod->GetOptimizer()->GetCurrentPosition();

  //
  // check results to see if it is within range
  //
  bool pass = true;
  double trueParameters[ImageDimension] = { -displacement[0], -displacement[1], -displacement[2] };
  for( unsigned int j = 0; j < ImageDimension; j++ )
    {
    if( vnl_math_abs( solution[j] - trueParameters[j] ) > 0.02 )
      pass = false;
    }
  
  if( !pass )
    {
    std::cout << "Test failed." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;





}



