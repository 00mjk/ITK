/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkGrayscaleDilateImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGrayscaleDilateImageFilter_txx
#define __itkGrayscaleDilateImageFilter_txx

#include "itkGrayscaleDilateImageFilter.h"

namespace itk {


template<class TInputImage, class TOutputImage, class TKernel>
GrayscaleDilateImageFilter<TInputImage, TOutputImage, TKernel>::PixelType
GrayscaleDilateImageFilter<TInputImage, TOutputImage, TKernel>
::Evaluate(const NeighborhoodIteratorType &nit,
           const KernelType &kernel)
{
  PixelType max = NumericTraits<PixelType>::NonpositiveMin();
  PixelType temp = max ;

  typename NeighborhoodIteratorType::ConstIterator neigh_it;
  KernelIteratorType kernel_it;
  const KernelIteratorType kernelEnd = kernel.End();

  neigh_it = nit.Begin();
  for (kernel_it=kernel.Begin(); kernel_it<kernelEnd; ++kernel_it, ++neigh_it)
    {
    // if structuring element is positive, use the pixel under that element
    // in the image
    if (*kernel_it > 0)
      {
      temp = **neigh_it;

      if (temp > max)
        max = temp ;
      }
    }
  
  return max ;
} 

template<class TInputImage, class TOutputImage, class TKernel>
GrayscaleDilateImageFilter<TInputImage, TOutputImage, TKernel>::PixelType
GrayscaleDilateImageFilter<TInputImage, TOutputImage, TKernel>
::Evaluate(const SmartNeighborhoodIteratorType &nit,
           const KernelType &kernel)
{
  unsigned int i;
  PixelType max = NumericTraits<PixelType>::NonpositiveMin();
  PixelType temp = max ;

  KernelIteratorType kernel_it;
  const KernelIteratorType kernelEnd = kernel.End();

  for (i=0, kernel_it=kernel.Begin(); kernel_it<kernelEnd; ++kernel_it, ++i)
    {
    // if structuring element is positive, use the pixel under that element
    // in the image
    if (*kernel_it > 0)
      {
      // note we use GetPixel() on the SmartNeighborhoodIterator to
      // respect boundary conditions
      temp = nit.GetPixel(i);

      if (temp > max)
        max = temp ;
      }
    }
  
  return max ;
} 


}// end namespace itk
#endif
