/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkNeighborhoodConnectedImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNeighborhoodConnectedImageFilter_txx_
#define __itkNeighborhoodConnectedImageFilter_txx_

#include "itkNeighborhoodConnectedImageFilter.h"
#include "itkNeighborhoodBinaryThresholdImageFunction.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputImage, class TOutputImage>
NeighborhoodConnectedImageFilter<TInputImage, TOutputImage>
::NeighborhoodConnectedImageFilter()
{
  m_Lower = NumericTraits<InputImagePixelType>::NonpositiveMin();
  m_Upper = NumericTraits<InputImagePixelType>::max();
  m_Seed.Fill(0);
  m_ReplaceValue = NumericTraits<OutputImagePixelType>::One;
  m_Radius.Fill(1);
}

/**
 * Standard PrintSelf method.
 */
template <class TInputImage, class TOutputImage>
void
NeighborhoodConnectedImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Upper: " << m_Upper << std::endl;
  os << indent << "Lower: " << m_Lower << std::endl;
  os << indent << "ReplaceValue: "
     << static_cast<typename NumericTraits<OutputImagePixelType>::PrintType>(m_ReplaceValue)
     << std::endl;
  os << indent << "Radius: " << m_Radius << std::endl;
}

template <class TInputImage, class TOutputImage>
void 
NeighborhoodConnectedImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();
  if ( this->GetInput() )
    {
    InputImagePointer image =
        const_cast< InputImageType * >( this->GetInput() );
    image->SetRequestedRegionToLargestPossibleRegion();
    }
}

template <class TInputImage, class TOutputImage>
void 
NeighborhoodConnectedImageFilter<TInputImage,TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *output)
{
  Superclass::EnlargeOutputRequestedRegion(output);
  output->SetRequestedRegionToLargestPossibleRegion();
}

template <class TInputImage, class TOutputImage>
void 
NeighborhoodConnectedImageFilter<TInputImage,TOutputImage>
::GenerateData()
{
  typename Superclass::InputImageConstPointer inputImage  = this->GetInput();
  typename Superclass::OutputImagePointer     outputImage = this->GetOutput();

  // Zero the output
  outputImage->SetBufferedRegion( outputImage->GetRequestedRegion() );
  outputImage->Allocate();
  outputImage->FillBuffer ( NumericTraits<OutputImagePixelType>::Zero );
  
  typedef NeighborhoodBinaryThresholdImageFunction<InputImageType> FunctionType;
  typedef FloodFilledImageFunctionConditionalIterator<OutputImageType, FunctionType> IteratorType;

  typename FunctionType::Pointer function = FunctionType::New();
    function->SetInputImage ( inputImage );
    function->ThresholdBetween ( m_Lower, m_Upper );
    function->SetRadius (m_Radius);
  IteratorType it = IteratorType ( outputImage, function, m_Seed );

  while( !it.IsAtEnd())
    {
    it.Set(m_ReplaceValue);
    ++it;
    }
}


} // end namespace itk

#endif
