/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkCannySegmentationLevelSetFunction.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCannySegmentationLevelSetFunction_txx_
#define __itkCannySegmentationLevelSetFunction_txx_

#include "itkCannySegmentationLevelSetFunction.h"
#include "itkGradientImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace itk {

template <class TImageType, class TFeatureImageType>
void CannySegmentationLevelSetFunction<TImageType, TFeatureImageType>
::CalculateSpeedImage()
{
  // Create a distance transform to the canny edges
  this->CalculateDistanceImage();

  // Graft the distance transform into the Speed Image
  this->GetSpeedImage()->Graft( m_Distance->GetOutput() );

}

template <class TImageType, class TFeatureImageType>
void CannySegmentationLevelSetFunction<TImageType, TFeatureImageType>
::CalculateAdvectionImage()
{
  typename GradientImageFilter<ImageType, ScalarValueType, ScalarValueType>::Pointer
    gradient = GradientImageFilter<ImageType, ScalarValueType, ScalarValueType>::New();

  typedef typename GradientImageFilter<ImageType, ScalarValueType,
    ScalarValueType>::OutputImageType CovariantVectorImageType;

  typename MultiplyImageFilter<CovariantVectorImageType, ImageType,
    CovariantVectorImageType>::Pointer multiply =
    MultiplyImageFilter<CovariantVectorImageType, ImageType, CovariantVectorImageType>::New();
  
  // Create a distance transform to the canny edges
  this->CalculateDistanceImage();

  gradient->SetInput(m_Distance->GetOutput());
  gradient->Update();

  multiply->SetInput1(gradient->GetOutput());
  multiply->SetInput2(m_Distance->GetOutput());
  
  //  multiply->GraftOutput(dynamic_cast<CovariantVectorImageType *>(this->GetAdvectionImage()));
  multiply->Update();  

// Copy output to Advection Image
  ImageRegionIterator<VectorImageType> it(this->GetAdvectionImage(),
                                          this->GetAdvectionImage()->GetRequestedRegion());
  ImageRegionConstIterator<CovariantVectorImageType> it_a(multiply->GetOutput(),
                                                          this->GetAdvectionImage()->GetRequestedRegion());
  
  for (; ! it.IsAtEnd(); ++it, ++it_a)
    {
    it.Set(it_a.Get());
    }
}

template <class TImageType, class TFeatureImageType>
void CannySegmentationLevelSetFunction<TImageType, TFeatureImageType>
::CalculateDistanceImage()
{

  // Only cast if we need to
  if ( typeid(TImageType) == typeid(TFeatureImageType))
    {
    m_Canny->SetInput(this->GetFeatureImage());
    }
  else
    {
    m_Caster->SetInput(this->GetFeatureImage());
    m_Canny->SetInput(m_Caster->GetOutput());
    }

  m_Canny->SetThreshold(m_Threshold);
  m_Canny->SetVariance(m_Variance);
  m_Canny->SetMaximumError(0.01);
  m_Canny->SetOutsideValue(NumericTraits<ScalarValueType>::Zero);

  m_Distance->SetInput(m_Canny->GetOutput());
  m_Distance->GetOutput()->SetRequestedRegion(m_SpeedImage->GetRequestedRegion());
  m_Distance->Update();
}

} // end namespace itk


#endif
