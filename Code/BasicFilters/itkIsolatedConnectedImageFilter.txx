/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkIsolatedConnectedImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkIsolatedConnectedImageFilter_txx_
#define __itkIsolatedConnectedImageFilter_txx_

#include "itkIsolatedConnectedImageFilter.h"
#include "itkBinaryThresholdImageFunction.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputImage, class TOutputImage>
IsolatedConnectedImageFilter<TInputImage, TOutputImage>
::IsolatedConnectedImageFilter()
{
  m_Lower = NumericTraits<InputImagePixelType>::NonpositiveMin();
  m_Seed1.Fill(0);
  m_Seed2.Fill(0);
  m_ReplaceValue = NumericTraits<OutputImagePixelType>::One;
}

/**
 * Standard PrintSelf method.
 */
template <class TInputImage, class TOutputImage>
void
IsolatedConnectedImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Lower: " << m_Lower << std::endl;
  os << indent << "ReplaceValue: "
     << static_cast<NumericTraits<OutputImagePixelType>::PrintType>(m_ReplaceValue)
     << std::endl;
  os << indent << "Seed1: " << m_Seed1 << std::endl;
  os << indent << "Seed2: " << m_Seed1 << std::endl;
  os << indent << "IsolatedValue: " << m_Seed1 << std::endl;
}

template <class TInputImage, class TOutputImage>
void 
IsolatedConnectedImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();
  if ( this->GetInput() )
    {
    InputImagePointer image = 
       const_cast< TInputImage * >( this->GetInput() );
    image->SetRequestedRegionToLargestPossibleRegion();
    }
}

template <class TInputImage, class TOutputImage>
void 
IsolatedConnectedImageFilter<TInputImage,TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *output)
{
  Superclass::EnlargeOutputRequestedRegion(output);
  output->SetRequestedRegionToLargestPossibleRegion();
}

template <class TInputImage, class TOutputImage>
void 
IsolatedConnectedImageFilter<TInputImage,TOutputImage>
::GenerateData()
{
  InputImageConstPointer inputImage = this->GetInput();
  OutputImagePointer outputImage = this->GetOutput();

  // Zero the output
  outputImage->SetBufferedRegion( outputImage->GetRequestedRegion() );
  outputImage->Allocate();
  outputImage->FillBuffer ( NumericTraits<OutputImagePixelType>::Zero );
  
  typedef BinaryThresholdImageFunction<InputImageType> FunctionType;
  typedef FloodFilledImageFunctionConditionalIterator<OutputImageType, FunctionType> IteratorType;

  FunctionType::Pointer function = FunctionType::New();
    function->SetInputImage ( inputImage );

  InputImagePixelType lower = m_Lower;
  InputImagePixelType upper = NumericTraits<InputImagePixelType>::max();
  InputImagePixelType guess = upper;
  IteratorType it = IteratorType ( outputImage, function, m_Seed1 );

  // do a binary search to find an upper threshold that separates the
  // two seeds.
  itkDebugMacro (<< "GetPixel(m_Seed1): " << inputImage->GetPixel(m_Seed1));
  itkDebugMacro (<< "GetPixel(m_Seed2): " << inputImage->GetPixel(m_Seed2));
  while (lower < guess)
    {
    itkDebugMacro( << "lower, upper, guess: " << lower << ", " << upper << ", " << guess);
    outputImage->FillBuffer ( NumericTraits<OutputImagePixelType>::Zero );
    function->ThresholdBetween ( m_Lower, guess );
    it.GoToBegin();
    while( !it.IsAtEnd())
      {
      it.Set(m_ReplaceValue);
      ++it;
      }
    if (outputImage->GetPixel(m_Seed2) == m_ReplaceValue)
      {
      upper = guess;
      }
    else
      {
      lower = guess;
      }
    guess = (upper + lower) /2;
    }

  // now rerun the algorithm with the threshold that separates the seeds.
  outputImage->FillBuffer ( NumericTraits<OutputImagePixelType>::Zero );
  function->ThresholdBetween ( m_Lower, lower);
  it.GoToBegin();
  while( !it.IsAtEnd())
    {
    it.Set(m_ReplaceValue);
    ++it;
    }
  m_IsolatedValue = lower;
}

} // end namespace itk

#endif
