/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkReinitializeLevelSetImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef _itkReinitializeLevelSetImageFilter_txx
#define _itkReinitializeLevelSetImageFilter_txx

#include "itkImageRegionIterator.h"
#include "itkIndex.h"

namespace itk
{

/**
 * Default constructor.
 */
template <class TLevelSet>
ReinitializeLevelSetImageFilter<TLevelSet>
::ReinitializeLevelSetImageFilter()
{

  m_LevelSetValue = 0.0;
  
  m_Locator = LocatorType::New();
  m_Marcher = FastMarchingImageFilterType::New();

  m_NarrowBanding = false;
  m_InputNarrowBandwidth = 12.0;
  m_OutputNarrowBandwidth = 12.0;
  m_InputNarrowBand = NULL;
  m_OutputNarrowBand = NULL;

}


/**
 * Set the input narrowband container.
 */
template <class TLevelSet>
void
ReinitializeLevelSetImageFilter<TLevelSet>
::SetInputNarrowBand(
NodeContainer * ptr )
{
  if( m_InputNarrowBand != ptr )
    {
    m_InputNarrowBand = ptr;
    this->Modified();
    }
}


/**
 * PrintSelf method.
 */
template <class TLevelSet>
void
ReinitializeLevelSetImageFilter<TLevelSet>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Level set value: " << m_LevelSetValue << std::endl;
  os << indent << "Narrowbanding: " << m_NarrowBanding << std::endl;
  os << indent << "Input narrow bandwidth: " << m_InputNarrowBandwidth;
  os << std::endl;
  os << indent << "Output narrow bandwidth: " << m_OutputNarrowBandwidth;
  os << std::endl;
  os << indent << "Input narrow band: " << m_InputNarrowBand.GetPointer();
  os << std::endl;
  os << indent << "Output narrow band: " << m_OutputNarrowBand.GetPointer();
  os << std::endl;

}


/**
 * GenerateInputRequestedRegion method.
 */
template <class TLevelSet>
void
ReinitializeLevelSetImageFilter<TLevelSet>
::GenerateInputRequestedRegion()
{
  // use the default implementation.
  this->Superclass::GenerateInputRequestedRegion();
}


/**
 * EnlargeOutputRequestedRegion method.
 */
template <class TLevelSet>
void
ReinitializeLevelSetImageFilter<TLevelSet>
::EnlargeOutputRequestedRegion(
DataObject *output )
{

  // this filter requires the all of the output image to be in
  // the buffer
  TLevelSet *imgData;
  imgData = dynamic_cast<TLevelSet*>( output );
  if ( imgData )
    {
    imgData->SetRequestedRegionToLargestPossibleRegion();
    }
  else
    {
    // pointer could not be cast to TLevelSet *
    itkWarningMacro(<< "itk::ReinitializeLevelSetImageFilter" <<
              "::EnlargeOutputRequestedRegion cannot cast "
              << typeid(output).name() << " to "
              << typeid(TLevelSet*).name() );

    }

}


/**
 * Allocate/initialize memory.
 */
template <class TLevelSet>
void
ReinitializeLevelSetImageFilter<TLevelSet>
::AllocateOutput()
{

  LevelSetPointer outputPtr = this->GetOutput();

  // allocate the output buffer memory 
  outputPtr->SetBufferedRegion(
    outputPtr->GetRequestedRegion() );
  outputPtr->Allocate();

  // set the marcher output size
  m_Marcher->SetOutputSize( 
    outputPtr->GetRequestedRegion().GetSize() );

}


/**
 * Generate the output data.
 */
template <class TLevelSet>
void
ReinitializeLevelSetImageFilter<TLevelSet>
::GenerateData()
{
  this->AllocateOutput();

  if( m_NarrowBanding )
    {
    this->GenerateDataNarrowBand();
    }
  else
    {
    this->GenerateDataFull();
    }

}

/**
 * Generate the output data - full set version.
 */
template <class TLevelSet>
void
ReinitializeLevelSetImageFilter<TLevelSet>
::GenerateDataFull()
{

  LevelSetPointer inputPtr = this->GetInput();
  LevelSetPointer outputPtr = this->GetOutput();
  LevelSetPointer tempLevelSet = m_Marcher->GetOutput();

  // define iterators
  typedef 
    ImageRegionIterator<LevelSetImageType> IteratorType;

  IteratorType inputIt( inputPtr,
    inputPtr->GetBufferedRegion() );
  IteratorType outputIt( outputPtr,
    outputPtr->GetBufferedRegion() );

  IteratorType tempIt;

  this->UpdateProgress( 0.0 );

  // locate the level set
  m_Locator->SetInputLevelSet( inputPtr );
  m_Locator->SetLevelSetValue( m_LevelSetValue );
  m_Locator->Locate();

  this->UpdateProgress( 0.33 );

  // march outward
  m_Marcher->SetTrialPoints( m_Locator->GetOutsidePoints() );
  m_Marcher->Update();

  tempIt = IteratorType( tempLevelSet,
    tempLevelSet->GetBufferedRegion() );

  double value;

  while( !inputIt.IsAtEnd() )
    {
    value = (double) inputIt.Get();
    if( value - m_LevelSetValue > 0 )
      {
      outputIt.Set( tempIt.Get() );
      }

    ++inputIt;
    ++outputIt;
    ++tempIt;
    }

  this->UpdateProgress( 0.66 );

  // march inward
  m_Marcher->SetTrialPoints( m_Locator->GetInsidePoints() );
  m_Marcher->Update();

  inputIt.GoToBegin();
  outputIt.GoToBegin();
  tempIt.GoToBegin();

  while( !inputIt.IsAtEnd() )
    {
    value = (double) inputIt.Get();
    if( value - m_LevelSetValue <= 0 )
      {
      value = (double) tempIt.Get();
      outputIt.Set( -1.0 * value );
      }

    ++inputIt;
    ++outputIt;
    ++tempIt;
    }

}
 
/**
 * Generate output data - narrowband version.
 */
template <class TLevelSet>
void
ReinitializeLevelSetImageFilter<TLevelSet>
::GenerateDataNarrowBand()
{

  LevelSetPointer inputPtr = this->GetInput();
  LevelSetPointer outputPtr = this->GetOutput();
  LevelSetPointer tempLevelSet = m_Marcher->GetOutput();

  // define iterators
  typedef 
    ImageRegionIterator<LevelSetImageType> IteratorType;

  IteratorType inputIt( inputPtr,
    inputPtr->GetBufferedRegion() );

  IteratorType outputIt( outputPtr,
    outputPtr->GetBufferedRegion() );

  PixelType posInfinity;
  PixelType negInfinity;

  posInfinity = NumericTraits<PixelType>::max();
  negInfinity = NumericTraits<PixelType>::NonpositiveMin();

  // set all internal pixels to minus infinity and 
  // all external pixels to positive infinity
  double value;

  while( !inputIt.IsAtEnd() )
    {
    value = (double) inputIt.Get();
    if( value - m_LevelSetValue <= 0 )
      {
      outputIt.Set( negInfinity );
      }
    else
      {
      outputIt.Set( posInfinity );
      }

    ++inputIt;
    ++outputIt;
    }


  // create a new output narrowband container
  m_OutputNarrowBand = NodeContainer::New();

  this->UpdateProgress( 0.0 );

  // locate the level set
  m_Locator->SetInputLevelSet( inputPtr );
  m_Locator->SetLevelSetValue( m_LevelSetValue );

  if( m_NarrowBanding && m_InputNarrowBand )
    {
    m_Locator->NarrowBandingOn();
    m_Locator->SetNarrowBandwidth( m_InputNarrowBandwidth );
    m_Locator->SetInputNarrowBand( m_InputNarrowBand );
    }
  else
    { 
    m_Locator->NarrowBandingOff();
    }

  m_Locator->Locate();

  this->UpdateProgress( 0.33 );

  // march outward
  double stoppingValue = ( m_OutputNarrowBandwidth / 2.0 ) + 2.0;
  m_Marcher->SetStoppingValue( stoppingValue );
  m_Marcher->CollectPointsOn();
  m_Marcher->SetTrialPoints( m_Locator->GetOutsidePoints() );
  m_Marcher->Update();

  NodeContainerPointer procPoints = m_Marcher->GetProcessedPoints();
  
  typename NodeContainer::ConstIterator pointsIt;
  typename NodeContainer::ConstIterator pointsEnd;
  
  pointsIt = procPoints->Begin();
  pointsEnd = procPoints->End();

  NodeType node;
  PixelType inPixel;

  for( ; pointsIt != pointsEnd; ++pointsIt )
    {
    node = pointsIt.Value();
    inPixel = inputPtr->GetPixel( node.GetIndex() );
    
    value = (double) inPixel;
    if( value - m_LevelSetValue > 0 )
      {
      inPixel = tempLevelSet->GetPixel( node.GetIndex() );
      outputPtr->SetPixel( node.GetIndex(), inPixel );
      m_OutputNarrowBand->InsertElement( m_OutputNarrowBand->Size(), node );

      }

  } // end for loop

  this->UpdateProgress( 0.66 );

  // march inward
  m_Marcher->SetTrialPoints( m_Locator->GetInsidePoints() );
  m_Marcher->Update();

  procPoints = m_Marcher->GetProcessedPoints();
  pointsIt = procPoints->Begin();
  pointsEnd = procPoints->End();

  for( ; pointsIt != pointsEnd; ++pointsIt )
    {
    node = pointsIt.Value();
    inPixel = inputPtr->GetPixel( node.GetIndex() );
    
    value = (double) inPixel ;
    if( value - m_LevelSetValue <= 0 )
      {
      inPixel = tempLevelSet->GetPixel( node.GetIndex() );
      value = (double) inPixel;
      inPixel =  -1.0 * value;
      outputPtr->SetPixel( node.GetIndex(), inPixel );
      node.SetValue( node.GetValue() * -1.0 );
      m_OutputNarrowBand->InsertElement( m_OutputNarrowBand->Size(), node );

      }

    } // end for loop

}


} // namespace itk

#endif
