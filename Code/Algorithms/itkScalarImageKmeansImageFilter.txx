/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkScalarImageKmeansImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkScalarImageKmeansImageFilter_txx
#define _itkScalarImageKmeansImageFilter_txx
#include "itkScalarImageKmeansImageFilter.h"

#include "itkProgressReporter.h"

namespace itk
{

template <class TInputImage>
ScalarImageKmeansImageFilter<TInputImage>
::ScalarImageKmeansImageFilter()
{
}

template< class TInputImage >
void
ScalarImageKmeansImageFilter< TInputImage >
::GenerateData()
{
  typename AdaptorType::Pointer adaptor = AdaptorType::New();
  adaptor->SetImage( this->GetInput() );

  typename TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();

  treeGenerator->SetSample( adaptor );
  treeGenerator->SetBucketSize( 16 );
  treeGenerator->Update();

  typename EstimatorType::Pointer estimator = EstimatorType::New();

  const unsigned int numberOfClasses = this->m_InitialMeans.size();

  ParametersType  initialMeans( numberOfClasses );
  for(unsigned int cl=0; cl<numberOfClasses; cl++)
    {
    initialMeans[0] = this->m_InitialMeans[cl];
    }

  estimator->SetParameters( initialMeans );
  
  estimator->SetKdTree( treeGenerator->GetOutput() );
  estimator->SetMaximumIteration( 200 );
  estimator->SetCentroidPositionChangesThreshold(0.0);
  estimator->StartOptimization();

  this->m_FinalMeans = estimator->GetParameters();

  typedef typename InputImageType::RegionType RegionType;
  typedef typename InputImageType::SizeType   SizeType;


  // Now classify the samples
  //
  typedef itk::Statistics::EuclideanDistance< MeasurementVectorType > 
                                                    MembershipFunctionType;

  typedef itk::MinimumDecisionRule DecisionRuleType;
  DecisionRuleType::Pointer decisionRule = DecisionRuleType::New();
  
  typedef itk::Statistics::SampleClassifier< AdaptorType > ClassifierType;
  typename ClassifierType::Pointer classifier = ClassifierType::New();

  classifier->SetDecisionRule( decisionRule.GetPointer() );
  classifier->SetSample( adaptor );

  classifier->SetNumberOfClasses( numberOfClasses  );

  std::vector< unsigned int > classLabels;
  classLabels.resize( numberOfClasses );

  // Spread the labels over the intensity range [0:255]
  const unsigned int labelInterval = ( 256 / numberOfClasses ) - 1;
  unsigned int label = 0;

  typedef typename MembershipFunctionType::Pointer     MembershipFunctionPointer;
  typedef typename MembershipFunctionType::OriginType  MembershipFunctionOriginType;

  for(unsigned int k=0; k<numberOfClasses; k++)
    {
    classLabels[k] = label;
    label += labelInterval;
    MembershipFunctionPointer membershipFunction = MembershipFunctionType::New();
    MembershipFunctionOriginType origin;
    origin[0] = this->m_FinalMeans[k]; // A scalar image has a MeasurementVector of dimension 1
    membershipFunction->SetOrigin( origin );
    classifier->AddMembershipFunction( membershipFunction.GetPointer() );
    }

  classifier->SetMembershipFunctionClassLabels( classLabels );

  // Execute the actual classification
  classifier->Update();


  // Now classify the pixels
  typename OutputImageType::Pointer outputPtr = this->GetOutput();

  typedef ImageRegionIterator< OutputImageType >  ImageIterator;

  outputPtr->SetBufferedRegion( outputPtr->GetRequestedRegion() );
  outputPtr->Allocate();

  RegionType region = outputPtr->GetBufferedRegion();

  ImageIterator pixel( outputPtr, region );
  pixel.GoToBegin();

  typedef typename ClassifierType::OutputType  ClassifierOutputType;
    
  ClassifierOutputType  * membershipSample = classifier->GetOutput();

  typedef typename ClassifierOutputType::ConstIterator LabelIterator;
  
  LabelIterator iter = membershipSample->Begin();
  LabelIterator end  = membershipSample->End();

  while ( iter != end )
    {
    pixel.Set( iter.GetClassLabel() );
    ++iter;
    ++pixel;
    }


}

/**
 * Add a new class for the classifier. This requires to explicitly set the
 * initial mean value for that class.
 */
template <class TInputImage >
void
ScalarImageKmeansImageFilter<TInputImage >
::AddClassWithInitialMean( RealPixelType mean )
{
  this->m_InitialMeans.push_back( mean );
}


/**
 * Standard "PrintSelf" method
 */
template <class TInputImage >
void
ScalarImageKmeansImageFilter<TInputImage >
::PrintSelf(
  std::ostream& os, 
  Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "Final Means " << m_FinalMeans << std::endl;
}

} // end namespace itk

#endif
