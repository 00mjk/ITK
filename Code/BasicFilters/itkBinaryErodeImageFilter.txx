/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBinaryErodeImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBinaryErodeImageFilter_txx
#define __itkBinaryErodeImageFilter_txx

#include "itkBinaryErodeImageFilter.h"

namespace itk {

template<class TInputImage, class TOutputImage, class TKernel>
BinaryErodeImageFilter<TInputImage, TOutputImage, TKernel>
::BinaryErodeImageFilter()
{
  m_ErodeValue = NumericTraits<PixelType>::NonpositiveMin();
  m_KernelCenterPixelOn = false;

  m_ErodeBoundaryCondition.SetConstant( NumericTraits<PixelType>::max() );
  this->OverrideBoundaryCondition( &m_ErodeBoundaryCondition );
}

template<class TInputImage, class TOutputImage, class TKernel>
void
BinaryErodeImageFilter<TInputImage, TOutputImage, TKernel>
::BeforeThreadedGenerateData()
{
  // Cache whether the center pixel in the kernel is set
  m_KernelCenterPixelOn = (this->GetKernel().GetCenterValue() > 0);
}

template<class TInputImage, class TOutputImage, class TKernel>
typename BinaryErodeImageFilter<TInputImage, TOutputImage, TKernel>::PixelType
BinaryErodeImageFilter<TInputImage, TOutputImage, TKernel>
::Evaluate(const NeighborhoodIteratorType &nit,
           const KernelIteratorType kernelBegin,
           const KernelIteratorType kernelEnd)
{
  unsigned int i;
  PixelType min = NumericTraits<PixelType>::max();
  PixelType nitValue;
  
  bool erode = false;               // do some sort of erosion
  bool completelyBackground = true; // structuring element is completely
                                    // over background pixels
  
  KernelIteratorType kernel_it;

  for (i=0, kernel_it=kernelBegin; kernel_it<kernelEnd; ++kernel_it, ++i)
    {
    // if structuring element is positive, use the pixel under that element
    // in the image
    if (*kernel_it > 0)
      {
      // if the image pixel is not the erode value, note we use GetPixel()
      // on the NeighborhoodIterator in order to respect boundary conditions
      nitValue = nit.GetPixel(i);
      if (nitValue != m_ErodeValue)
        {
        erode = true;
        
        // if the image pixel is less than current min, keep track of it
        if (min > nitValue)
          {
          min = nitValue;
          }
        }
      else
        {
        // at least one pixel in structuring element is the foreground
        completelyBackground = false;
        }
      }
    }

  // Four cases for the return value:
  // 1) If nothing in structuring element is the ErodeValue (foreground)
  //      then leave pixel unchanged
  // 2) If all of structuring element is the ErodeValue (foreground)
  //      then return ErodeValue
  // 3) If part of the structuring elemene is over background, and the
  //       center pixel of the structuring element is "on", then
  //       return the minimum of all the background values visited
  // 4) If part of the structuring element is over background, and the
  //       center pixel of the structuring element is "off", then
  //       leave pixel unchanged
  if (completelyBackground)
    {
    // case #1
    return nit.GetCenterPixel();
    }
  else
    {
    if (!erode)
      {
      // case #2, don't erode
      return m_ErodeValue;
      }
    else
      {
      if (m_KernelCenterPixelOn)
        {
        // case #3, center pixel is "on"
        return min;
        }
      else
        {
        // case #4, center pixel is "off"
        return nit.GetCenterPixel();
        }
      }
    }
} 


template<class TInputImage, class TOutputImage, class TKernel>
void
BinaryErodeImageFilter<TInputImage, TOutputImage, TKernel>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Erode value: "
     << static_cast<typename NumericTraits<PixelType>::PrintType>(m_ErodeValue)
     << std::endl;
}

}// end namespace itk
#endif
