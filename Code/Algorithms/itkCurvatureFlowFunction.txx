/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkCurvatureFlowFunction.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCurvatureFlowFunction_txx_
#define __itkCurvatureFlowFunction_txx_

#include "vnl/vnl_math.h"

namespace itk {

/**
 * Constructor
 */
template<class TImage>
CurvatureFlowFunction<TImage>
::CurvatureFlowFunction()
{

  RadiusType r;
  int j;
  for( j = 0; j < ImageDimension; j++ )
    {
    r[j] = 1;
    }

  this->SetRadius(r);

  m_TimeStep  = 0.125f;

}


/**
 * Update the solution at pixels which does not lie on the
 * data boundary.
 */
template<class TImage>
CurvatureFlowFunction<TImage>::PixelType
CurvatureFlowFunction<TImage>
::ComputeUpdate(const NeighborhoodType &it, void * globalData,
                const FloatOffsetType& offset) const
{
  
  PixelType firstderiv[ImageDimension];
  PixelType secderiv[ImageDimension];
  PixelType crossderiv[ImageDimension][ImageDimension];
  unsigned long center;
  unsigned long stride[ImageDimension];
  int i,j;

  // get the center pixel position
  center = it.Size() / 2;

  // cache the stride for each dimension
  for( i = 0; i < ImageDimension; i++ )
    {
    stride[i] = it.GetStride( (unsigned long) i );
    }

  PixelType magnitude = 0.0;
  for( i = 0; i < ImageDimension; i++ )
    {

    // compute first order derivatives
    firstderiv[i] = 0.5 * ( it.GetPixel(center + stride[i]) - 
      it.GetPixel(center - stride[i]) );

    // compute second order derivatives
    secderiv[i] = 0.5 * ( it.GetPixel(center + stride[i]) - 
      2 * it.GetPixel(center) + it.GetPixel( center - stride[i] ) );

    // compute cross derivatives
    for( j = i + 1; j < ImageDimension; j++ )
      {
      crossderiv[i][j] = 0.25 * (
           it.GetPixel( center - stride[i] - stride[j] ) 
         - it.GetPixel( center - stride[i] + stride[j] )
         - it.GetPixel( center + stride[i] - stride[j] ) 
         + it.GetPixel( center + stride[i] + stride[j] ) );
      }

    // accumlate the gradient magnitude
    magnitude += vnl_math_sqr( firstderiv[i] );

    }

  // compute the magnitude
  magnitude = vnl_math_sqrt( (double)magnitude );
  if ( magnitude < 1e-9 )
    {
    return NumericTraits<PixelType>::Zero; 
    }

  // compute the update value = mean curvature * magnitude
  PixelType update = 0.0;
  PixelType temp;

  // accumulate dx^2 * (dyy + dzz) terms
  for( i = 0; i < ImageDimension; i++ )
    {
    temp = 0.0;
    for( j = 0; j < ImageDimension; j++ )
      {
        if( j == i ) continue;
        temp += secderiv[j];
      }
    
    update += temp * vnl_math_sqr( firstderiv[i] );
    }

  // accumlate -2 * dx * dy * dxy terms
  for( i = 0; i < ImageDimension; i++ )
    {
    for( j = i + 1; j < ImageDimension; j++ )
      {
      update -= 2 * firstderiv[i] * firstderiv[j] *
        crossderiv[i][j];
      }
    }
   
  update /= vnl_math_sqr( magnitude );

  return update;

}


/**
 * Update the solution at pixels which lies on the data boundary.
 */
template<class TImage>
CurvatureFlowFunction<TImage>::PixelType
CurvatureFlowFunction<TImage>
::ComputeUpdate(const BoundaryNeighborhoodType &it, void * globalData,
                const FloatOffsetType& offset) const
{

  PixelType firstderiv[ImageDimension];
  PixelType secderiv[ImageDimension];
  PixelType crossderiv[ImageDimension][ImageDimension];
  unsigned long center;
  unsigned long stride[ImageDimension];
  int i,j;

  // get the center pixel position
  center = it.Size() / 2;

  // cache the stride for each dimension
  for( i = 0; i < ImageDimension; i++ )
    {
    stride[i] = it.GetStride( (unsigned long) i );
    }

  PixelType magnitude = 0.0;
  for( i = 0; i < ImageDimension; i++ )
    {

    // compute first order derivatives
    firstderiv[i] = 0.5 * ( it.GetPixel(center + stride[i]) - 
      it.GetPixel(center - stride[i]) );

    // compute second order derivatives
    secderiv[i] = 0.5 * ( it.GetPixel(center + stride[i]) - 
      2 * it.GetPixel(center) + it.GetPixel( center - stride[i] ) );

    // compute cross derivatives
    for( j = i + 1; j < ImageDimension; j++ )
      {
      crossderiv[i][j] = 0.25 * (
           it.GetPixel( center - stride[i] - stride[j] ) 
         - it.GetPixel( center - stride[i] + stride[j] )
         - it.GetPixel( center + stride[i] - stride[j] ) 
         + it.GetPixel( center + stride[i] + stride[j] ) );
      }

    // accumlate the gradient magnitude
    magnitude += vnl_math_sqr( firstderiv[i] );

    }

  // compute the magnitude
  magnitude = vnl_math_sqrt( (double)magnitude );
  if ( magnitude < 1e-9 )
    {
    return NumericTraits<PixelType>::Zero; 
    }

  // compute the update value = mean curvature * magnitude
  PixelType update = 0.0;
  PixelType temp;

  // accumulate dx^2 * (dyy + dzz) terms
  for( i = 0; i < ImageDimension; i++ )
    {
    temp = 0.0;
    for( j = 0; j < ImageDimension; j++ )
      {
        if( j == i ) continue;
        temp += secderiv[j];
      }
    
    update += temp * vnl_math_sqr( firstderiv[i] );
    }

  // accumlate -2 * dx * dy * dxy terms
  for( i = 0; i < ImageDimension; i++ )
    {
    for( j = i + 1; j < ImageDimension; j++ )
      {
      update -= 2 * firstderiv[i] * firstderiv[j] *
        crossderiv[i][j];
      }
    }
   
  update /= vnl_math_sqr( magnitude );

  return update;

}

} // end namespace itk

#endif
