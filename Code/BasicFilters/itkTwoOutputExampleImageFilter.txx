/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkTwoOutputExampleImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkTwoOutputExampleImageFilter_txx
#define _itkTwoOutputExampleImageFilter_txx

#include "itkTwoOutputExampleImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkNumericTraits.h"
#include "itkObjectFactory.h"

namespace itk
{

/**
 *
 */
template <class TImage>
TwoOutputExampleImageFilter<TImage>
::TwoOutputExampleImageFilter()
{
  m_OutsideValue = NumericTraits<PixelType>::Zero;
  m_Lower = NumericTraits<PixelType>::NonpositiveMin();
  m_Upper = NumericTraits<PixelType>::max();

  typename TImage::Pointer output = TImage::New();
  this->ProcessObject::SetNumberOfRequiredOutputs(2);
  this->ProcessObject::SetNthOutput(1, output.GetPointer());
}


/**
 *
 */
template <class TImage>
void 
TwoOutputExampleImageFilter<TImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Outside Value: " << m_OutsideValue << std::endl;
  os << indent << "Lower: " << m_Lower << std::endl;
  os << indent << "Upper: " << m_Upper << std::endl;
}

/**
 * The values greater than or equal to the value are set to OutsideValue
 */
template <class TImage>
void 
TwoOutputExampleImageFilter<TImage>
::ThresholdAbove(PixelType &thresh)
{
  if (m_Upper != thresh
      || m_Lower > NumericTraits<PixelType>::NonpositiveMin())
    {
    m_Lower = NumericTraits<PixelType>::NonpositiveMin();
    m_Upper = thresh;
    this->Modified();
    }
}



/**
 * The values less than or equal to the value are set to OutsideValue
 */
template <class TImage>
void 
TwoOutputExampleImageFilter<TImage>
::ThresholdBelow(PixelType &thresh)
{
  if (m_Lower != thresh || m_Upper < NumericTraits<PixelType>::max())
    {
    m_Lower = thresh;
    m_Upper = NumericTraits<PixelType>::max();
    this->Modified();
    }
}


/**
 * The values outside the range are set to OutsideValue
 */
template <class TImage>
void 
TwoOutputExampleImageFilter<TImage>
::ThresholdOutside(PixelType &lower, PixelType &upper)
{
  if (lower > upper)
    {
    itkExceptionMacro(<<"Lower threshold cannot be greater than upper threshold.");
    return;
    }
  
  if (m_Lower != lower || m_Upper != upper)
    {
    m_Lower = lower;
    m_Upper = upper;
    this->Modified();
    }
}


/**
 *
 */
template <class TImage>
void 
TwoOutputExampleImageFilter<TImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{
  unsigned long i;
  itkDebugMacro(<<"Actually executing");

  // Get the input and output pointers
  InputImagePointer  inputPtr = this->GetInput();
  OutputImagePointer outputPtr = this->GetOutput(0);
  OutputImagePointer outputInversePtr = this->GetOutput(1);

  // Define/declare an iterator that will walk the output region for this
  // thread.
  typedef
    ImageRegionIterator<TImage> InputIterator;
  typedef
    ImageRegionIterator<TImage> OutputIterator;

  InputIterator  inIt(inputPtr, outputRegionForThread);
  OutputIterator outIt(outputPtr, outputRegionForThread);
  OutputIterator outInverseIt(outputInversePtr, outputRegionForThread);

  // support progress methods/callbacks
  unsigned long updateVisits = 0;
  if ( threadId == 0 )
    {
    updateVisits = outputRegionForThread.GetNumberOfPixels()/10;
    if ( updateVisits < 1 ) updateVisits = 1;
    }
        
  // walk the regions, threshold each pixel
  for (i=0; !outIt.IsAtEnd(); ++inIt, ++outIt, ++outInverseIt, i++ )
    {
    if ( threadId == 0 && !(i % updateVisits ) )
      {
      this->UpdateProgress((float)i/(float(updateVisits)*10.0));
      }

    if (m_Lower <= inIt.Get() && inIt.Get() <= m_Upper)
      {
      // pixel passes to output unchanged and is replaced by m_OutsideValue in
      // the inverse output image
      outIt.Set( inIt.Get() );
      outInverseIt.Set( m_OutsideValue );
      }
    else
      {
      outIt.Set( m_OutsideValue );
      outInverseIt.Set( inIt.Get() );
      }
    }
  
}

} // end namespace itk

#endif
