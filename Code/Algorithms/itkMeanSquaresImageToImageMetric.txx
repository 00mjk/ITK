/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkMeanSquaresImageToImageMetric.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkMeanSquaresImageToImageMetric_txx
#define _itkMeanSquaresImageToImageMetric_txx

#include "itkMeanSquaresImageToImageMetric.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{

/*
 * Constructor
 */
template <class TFixedImage, class TMovingImage> 
MeanSquaresImageToImageMetric<TFixedImage,TMovingImage>
::MeanSquaresImageToImageMetric()
{
}

/*
 * Get the match Measure
 */
template <class TFixedImage, class TMovingImage> 
MeanSquaresImageToImageMetric<TFixedImage,TMovingImage>::MeasureType
MeanSquaresImageToImageMetric<TFixedImage,TMovingImage>
::GetValue( const TransformParametersType & parameters ) const
{

  FixedImageConstPointer fixedImage = this->GetFixedImage();

  if( !fixedImage ) 
    {
    itkExceptionMacro( << "Fixed image has not been assigned" );
    }

  typedef  itk::ImageRegionConstIteratorWithIndex<FixedImageType> FixedIteratorType;


  FixedIteratorType ti( fixedImage, this->GetFixedImageRegion() );

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
      const RealType movingValue  = m_Interpolator->Evaluate( transformedPoint );
      const RealType fixedValue   = ti.Get();
      m_NumberOfPixelsCounted++;
      const RealType diff = movingValue - fixedValue; 
      measure += diff * diff; 
      }

    ++ti;
    }

  if( !m_NumberOfPixelsCounted )
    {
    itkExceptionMacro(<<"All the points mapped to outside of the moving image");
    }
  else
    {
    measure /= m_NumberOfPixelsCounted;
    }

  return measure;

}





/*
 * Get the Derivative Measure
 */
template < class TFixedImage, class TMovingImage> 
void
MeanSquaresImageToImageMetric<TFixedImage,TMovingImage>
::GetDerivative( const TransformParametersType & parameters,
                       DerivativeType & derivative  ) const
{

  if( !m_GradientImage )
    {
    itkExceptionMacro(<<"The gradient image is null, maybe you forgot to call Initialize()");
    }

  FixedImageConstPointer fixedImage = this->GetFixedImage();

  if( !fixedImage ) 
    {
    itkExceptionMacro( << "Fixed image has not been assigned" );
    }

  const unsigned int ImageDimension = FixedImageType::ImageDimension;


  typedef  itk::ImageRegionConstIteratorWithIndex<
                                          FixedImageType> FixedIteratorType;

  typedef  itk::ImageRegionConstIteratorWithIndex<
                                        GradientImageType> GradientIteratorType;


  FixedIteratorType ti( fixedImage, this->GetFixedImageRegion() );

  GradientIteratorType gi( m_GradientImage, this->GetFixedImageRegion() );

  typename FixedImageType::IndexType index;

  m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  const unsigned int ParametersDimension = this->GetNumberOfParameters();
  derivative = DerivativeType( ParametersDimension );
  derivative.Fill( NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );

  ti.GoToBegin();
  gi.GoToBegin();

  while(!ti.IsAtEnd())
    {

    index = ti.GetIndex();
    
    InputPointType inputPoint;
    fixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

    OutputPointType transformedPoint = m_Transform->TransformPoint( inputPoint );

    if( m_Interpolator->IsInsideBuffer( transformedPoint ) )
      {
      const RealType movingValue  = m_Interpolator->Evaluate( transformedPoint );

      const TransformJacobianType & jacobian =
                          m_Transform->GetJacobian( inputPoint ); 

      
      const RealType fixedValue     = ti.Value();
      m_NumberOfPixelsCounted++;
      const RealType diff = movingValue - fixedValue; 

      const GradientPixelType & gradient = gi.Value(); 
      for(unsigned int par=0; par<ParametersDimension; par++)
        {
        RealType sum = NumericTraits< RealType >::Zero;
        for(unsigned int dim=0; dim<ImageDimension; dim++)
          {
          sum += 2.0 * diff * jacobian( dim, par ) * gradient[dim];
          }
        derivative[par] += sum;
        }
      }

    ++ti;
    ++gi;
    }

  if( !m_NumberOfPixelsCounted )
    {
    itkExceptionMacro(<<"All the points mapped to outside of the moving image");
    }
  else
    {
    for(unsigned int i=0; i<ParametersDimension; i++)
      {
      derivative[i] /= m_NumberOfPixelsCounted;
      }
    }

}


/*
 * Get both the match Measure and theDerivative Measure 
 */
template <class TFixedImage, class TMovingImage> 
void
MeanSquaresImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivative(const TransformParametersType & parameters, 
                        MeasureType & value, DerivativeType  & derivative) const
{

  if( !m_GradientImage )
    {
    itkExceptionMacro(<<"The gradient image is null, maybe you forgot to call Initialize()");
    }

  FixedImageConstPointer fixedImage = this->GetFixedImage();

  if( !fixedImage ) 
    {
    itkExceptionMacro( << "Fixed image has not been assigned" );
    }

  const unsigned int ImageDimension = FixedImageType::ImageDimension;

  typedef  itk::ImageRegionConstIteratorWithIndex<
                                          FixedImageType> FixedIteratorType;

  typedef  itk::ImageRegionConstIteratorWithIndex<
                                        GradientImageType> GradientIteratorType;


  FixedIteratorType ti( fixedImage, this->GetFixedImageRegion() );

  GradientIteratorType gi( m_GradientImage, this->GetFixedImageRegion() );

  typename FixedImageType::IndexType index;

  MeasureType measure = NumericTraits< MeasureType >::Zero;

  m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  const unsigned int ParametersDimension = this->GetNumberOfParameters();
  derivative = DerivativeType( ParametersDimension );
  derivative.Fill( NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );

  ti.GoToBegin();
  gi.GoToBegin();

  while(!ti.IsAtEnd())
    {

    index = ti.GetIndex();
    
    InputPointType inputPoint;
    fixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

    OutputPointType transformedPoint = m_Transform->TransformPoint( inputPoint );

    if( m_Interpolator->IsInsideBuffer( transformedPoint ) )
      {
      const RealType movingValue  = m_Interpolator->Evaluate( transformedPoint );

      const TransformJacobianType & jacobian =
                          m_Transform->GetJacobian( inputPoint ); 

      
      const RealType fixedValue     = ti.Value();
      m_NumberOfPixelsCounted++;

      const RealType diff = movingValue - fixedValue; 
  
      measure += diff * diff;

      const GradientPixelType & gradient = gi.Value(); 
      for(unsigned int par=0; par<ParametersDimension; par++)
        {
        RealType sum = NumericTraits< RealType >::Zero;
        for(unsigned int dim=0; dim<ImageDimension; dim++)
          {
          sum += 2.0 * diff * jacobian( dim, par ) * gradient[dim];
          }
        derivative[par] += sum;
        }
      }

    ++ti;
    ++gi;
    }

  if( !m_NumberOfPixelsCounted )
    {
    itkExceptionMacro(<<"All the points mapped to outside of the moving image");
    }
  else
    {
    for(unsigned int i=0; i<ParametersDimension; i++)
      {
      derivative[i] /= m_NumberOfPixelsCounted;
      }
    measure /= m_NumberOfPixelsCounted;
    }

  value = measure;

}

} // end namespace itk


#endif
