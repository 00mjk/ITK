/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkPatternIntensityImageToImageMetric.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkPatternIntensityImageToImageMetric_txx
#define _itkPatternIntensityImageToImageMetric_txx

#include "itkPatternIntensityImageToImageMetric.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{

/**
 * Constructor
 */
template <class TFixedImage, class TMovingImage> 
PatternIntensityImageToImageMetric<TFixedImage,TMovingImage>
::PatternIntensityImageToImageMetric()
{
}

/**
 * Get the match Measure
 */
template <class TFixedImage, class TMovingImage> 
PatternIntensityImageToImageMetric<TFixedImage,TMovingImage>::MeasureType
PatternIntensityImageToImageMetric<TFixedImage,TMovingImage>
::GetValue( const TransformParametersType & parameters ) const
{

  FixedImageConstPointer fixedImage = this->GetFixedImage();

  if( !fixedImage ) 
    {
    itkExceptionMacro( << "Fixed image has not been assigned" );
    }

  typename FixedImageType::RegionType  fixedRegion = 
                              fixedImage->GetLargestPossibleRegion();

  const unsigned int dimension = FixedImageType::ImageDimension;
  itk::Point<double, dimension> Point;  

  double MovingValue;
  double FixedValue;

  typedef  itk::ImageRegionConstIteratorWithIndex<FixedImageType> FixedIteratorType;


  FixedIteratorType ti( fixedImage, fixedRegion );

  typename FixedImageType::IndexType index;

  MeasureType measure = NumericTraits< MeasureType >::Zero;

  m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  while(!ti.IsAtEnd())
    {

    index = ti.GetIndex();
    
    InputPointType inputPoint;
    fixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

    OutputPointType transformedPoint = m_Transform->TransformPoint( inputPoint );

    if( m_Interpolator->IsInsideBuffer( transformedPoint ) )
      {
      MovingValue  = m_Interpolator->Evaluate( transformedPoint );
      FixedValue     = ti.Get();
      m_NumberOfPixelsCounted++;
      const double diff = MovingValue - FixedValue; 
      measure += 1.0f / ( 1.0f + diff * diff ); 
      }

    ++ti;
    }

  return measure;

}





/**
 * Get the Derivative Measure
 */
template < class TFixedImage, class TMovingImage> 
void
PatternIntensityImageToImageMetric<TFixedImage,TMovingImage>
::GetDerivative( const TransformParametersType & parameters, 
                       DerivativeType & derivative           ) const
{

  const double delta = 0.00011;
  TransformParametersType testPoint;
  testPoint = parameters;

  derivative = DerivativeType( this->GetNumberOfParameters() );

  const unsigned int dimension = FixedImageType::ImageDimension;
  for( unsigned int i=0; i<dimension; i++) 
    {
    testPoint[i] -= delta;
    const MeasureType valuep0 = this->GetValue( testPoint );
    testPoint[i] += 2*delta;
    const MeasureType valuep1 = this->GetValue( testPoint );
    derivative[i] = (valuep1 - valuep0 ) / ( 2 * delta );
    testPoint[i] = parameters[i];
    }

  

}


/**
 * Get both the match Measure and theDerivative Measure 
 */
template <class TFixedImage, class TMovingImage> 
void
PatternIntensityImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivative(const TransformParametersType & parameters, 
                        MeasureType & Value, DerivativeType  & Derivative) const
{
  Value      = this->GetValue( parameters );
  this->GetDerivative( parameters, Derivative );
}

} // end namespace itk


#endif
