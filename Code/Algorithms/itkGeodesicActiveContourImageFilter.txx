/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkGeodesicActiveContourImageFilter.txx
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
#ifndef _itkGeodesicActiveContourImageFilter_txx
#define _itkGeodesicActiveContourImageFilter_txx

#include "itkLevelSetCurvatureFunction.h"
#include "itkEntropyPreservingGradientMagnitudeImageFunction.h"
#include "itkUpwindDerivativeImageFunction.h"
#include "itkImageRegionIterator.h"

#include "vnl/vnl_math.h"

namespace itk
{

/**
 *
 */
template <class TLevelSet, class TEdgeImage, class TDerivImage>
GeodesicActiveContourImageFilter<TLevelSet,TEdgeImage,TDerivImage>
::GeodesicActiveContourImageFilter()
{
  m_Extender = ExtenderType::New();

  for( unsigned int j = 0; j < SetDimension; j++ )
    {
    m_DerivImages[j] = NULL;  
    }

  m_InflationStrength = 0.0;

}


/**
 *
 */
template <class TLevelSet, class TEdgeImage, class TDerivImage>
void
GeodesicActiveContourImageFilter<TLevelSet,TEdgeImage,TDerivImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Inflation strength: " << m_InflationStrength;
  os << std::endl;
  os << indent << "Edge image derivatives: [";
  int j;
  for ( j = 0; j < SetDimension - 1; j++ )
    {
    os << m_DerivImages[j].GetPointer() << ", ";
    }
  os << m_DerivImages[j].GetPointer() << "]" << std::endl;

}

/**
 *
 */
template <class TLevelSet, class TEdgeImage, class TDerivImage>
void
GeodesicActiveContourImageFilter<TLevelSet,TEdgeImage,TDerivImage>
::SetDerivativeImage(
TDerivImage * ptr,
unsigned int dim )
{
  if( !ptr || dim > SetDimension - 1) return;

  m_DerivImages[dim] = ptr;
  this->ProcessObject::SetNthInput( dim+1, ptr );

}


/**
 *
 */
template <class TLevelSet, class TEdgeImage, class TDerivImage>
void
GeodesicActiveContourImageFilter<TLevelSet,TEdgeImage,TDerivImage>
::GenerateInputRequestedRegion()
{
  //call the superclass implementation of this method
  this->Superclass::GenerateInputRequestedRegion();

  // this filter requires all of the input images to
  // be in the buffer
  for( unsigned int k = 0; k < SetDimension; k++ )
    {
    DerivImagePointer ptr = this->GetInput( k+1 );
    ptr->SetRequestedRegionToLargestPossibleRegion();
    }

}


/**
 *
 */
template <class TLevelSet, class TEdgeImage, class TDerivImage>
void
GeodesicActiveContourImageFilter<TLevelSet,TEdgeImage,TDerivImage>
::GenerateDataFull()
{
  
  for( unsigned int j = 0; j < SetDimension; j++ )
    {
    if ( !m_DerivImages[j] )
      {
        throw ExceptionObject(__FILE__, __LINE__);
      }
    }

  this->AllocateBuffers();
  this->CopyInputToInputBuffer();

  EdgeImagePointer edgeImage = this->GetEdgeImage();

  unsigned int numberOfIterations = this->GetNumberOfIterations();
  double timeStepSize = this->GetTimeStepSize();
  bool propagateOutwards = this->GetPropagateOutwards();

  // Define a level set curvature calculator
  typedef
    LevelSetCurvatureFunction<LevelSetImageType> CurvatureType;
  CurvatureType::Pointer inCurvature = CurvatureType::New();

  // Define a entropy-satisfying derivative calculator
  typedef
    EntropyPreservingGradientMagnitudeImageFunction<LevelSetImageType> DerivativeType;
  DerivativeType::Pointer inEntropy = DerivativeType::New();
  if( propagateOutwards )
    {
    inEntropy->SetSpeed( 1.0 );
    }
  else
    {
    inEntropy->SetSpeed( -1.0 );
    }

  // Define a upwind-derivative calculator
  typedef
    UpwindDerivativeImageFunction<LevelSetImageType> UpwindType;
  UpwindType::Pointer inUpwind = UpwindType::New();


  for( unsigned int k = 0; k < numberOfIterations; k++ )
    {
      
    itkDebugMacro(<< "iteration: " << k);

    LevelSetPointer inputBuffer = this->GetInputBuffer();
    LevelSetPointer outputBuffer = this->GetOutputBuffer();

    typedef Index<TLevelSet::ImageDimension> IndexType;

    inCurvature->SetInputImage( inputBuffer );
    inEntropy->SetInputImage( inputBuffer );
    inUpwind->SetInputImage( inputBuffer );

    // Define iterators
    typedef
       ImageRegionIterator<LevelSetImageType> IteratorType;
  
    IteratorType inIt = IteratorType( inputBuffer, 
      inputBuffer->GetBufferedRegion() );
    IteratorType outIt = IteratorType( outputBuffer, 
      outputBuffer->GetBufferedRegion() );

    typedef
      ImageRegionIterator<EdgeImageType> 
        SpeedIteratorType;

    SpeedIteratorType speedIt = SpeedIteratorType( edgeImage, 
      edgeImage->GetBufferedRegion() );

    typedef
      ImageRegionIterator<DerivImageType> 
        DerivIteratorType;
    DerivIteratorType derivIt[SetDimension];

    for( unsigned int j = 0; j < SetDimension; j++ )
      {
      derivIt[j] = DerivIteratorType( m_DerivImages[j], 
        m_DerivImages[j]->GetBufferedRegion() );
      }

    IndexType index;
    double curvature;
    double magnitude;
    double updateValue;
    double speed;
    double deriv;
    double value;

    outIt.GoToBegin();
    inIt.GoToBegin();
    speedIt.GoToBegin();
    for( unsigned int j = 0; j < SetDimension; j++ )
      {
      derivIt[j].GoToBegin();
      }

    while( !outIt.IsAtEnd() )
      {
      index = inIt.GetIndex();
    
      magnitude = inEntropy->EvaluateAtIndex( index );
      updateValue = m_InflationStrength * magnitude;
      if( propagateOutwards )
      {
        updateValue *= -1.0;
      }

      curvature = inCurvature->EvaluateAtIndex( index );
      magnitude = inCurvature->GetMagnitude();
      updateValue += curvature * magnitude;

      speed = (double) speedIt.Get();
      updateValue *= speed;

      for( unsigned int j = 0; j < SetDimension; j++ )
        {
        deriv = (double) derivIt[j].Get();

        inUpwind->SetSpeed( -1.0 * deriv );

        updateValue += deriv * inUpwind->EvaluateAtIndex( index, j );
        }

      updateValue *= timeStepSize; 
    
      value = (double) inIt.Get();
      value += updateValue;

      outIt.Set( value );

      ++outIt;
      ++inIt;
      ++speedIt;
      for( unsigned int j = 0; j < SetDimension; j++ )
        {
        ++(derivIt[j]);
        }

      } // end while loop

    this->SwapBuffers();

    } // end iteration loop

  this->SwapBuffers();
  this->CopyOutputBufferToOutput();
                                        
}

/**
 *
 */
template <class TLevelSet, class TEdgeImage, class TDerivImage>
void
GeodesicActiveContourImageFilter<TLevelSet,TEdgeImage,TDerivImage>
::GenerateDataNarrowBand()
{

  for( unsigned int j = 0; j < SetDimension; j++ )
    {
    if ( !m_DerivImages[j] )
      {
        throw ExceptionObject(__FILE__, __LINE__);
      }
    }

  this->AllocateBuffers(true);
  
  LevelSetPointer outputPtr = this->GetOutputBuffer();
  LevelSetPointer inputPtr = this->GetInput();
  EdgeImagePointer edgeImage = this->GetEdgeImage();

  // copy input to output
  typedef
     ImageRegionIterator<LevelSetImageType> IteratorType;
  
  IteratorType inIt = IteratorType( 
    inputPtr, inputPtr->GetBufferedRegion() );
  IteratorType outIt = IteratorType( 
    outputPtr, outputPtr->GetBufferedRegion() );

  while( !inIt.IsAtEnd() )
    {
    outIt.Set( inIt.Get() );
    ++inIt;
    ++outIt;
    }

  double narrowBandwidth = this->GetNarrowBandwidth();

  // Setup the extender
  m_Extender->NarrowBandingOn();
  m_Extender->SetNarrowBandwidth( narrowBandwidth );

  NodeContainerPointer inputNarrowBand = this->GetInputNarrowBand();
  unsigned int numberOfIterations = this->GetNumberOfIterations();
  double timeStepSize = this->GetTimeStepSize();
  bool propagateOutwards = this->GetPropagateOutwards();

  // Define a level set curvature calculator
  typedef
    LevelSetCurvatureFunction<LevelSetImageType> CurvatureType;
  CurvatureType::Pointer inCurvature = CurvatureType::New();

  // Define a entropy-satisfying derivative calculator
  typedef
    EntropyPreservingGradientMagnitudeImageFunction<LevelSetImageType> DerivativeType;
  DerivativeType::Pointer inEntropy = DerivativeType::New();
  if( propagateOutwards )
    {
    inEntropy->SetSpeed( 1.0 );
    }
  else
    {
    inEntropy->SetSpeed( -1.0 );
    }

  // Define a upwind-derivative calculator
  typedef
    UpwindDerivativeImageFunction<LevelSetImageType> UpwindType;
  UpwindType::Pointer inUpwind = UpwindType::New();
 
  for( unsigned int k = 0; k < numberOfIterations; k++ )
    {

    itkDebugMacro(<< "iteration: " << k);

    m_Extender->SetInput( outputPtr );
    m_Extender->SetInputNarrowBand( inputNarrowBand );
    m_Extender->Update();

    inputNarrowBand = m_Extender->GetOutputNarrowBand();
    inputPtr = m_Extender->GetOutput();

    inCurvature->SetInputImage( inputPtr );
    inEntropy->SetInputImage( inputPtr );
    inUpwind->SetInputImage( inputPtr );

    typename NodeContainer::ConstIterator pointsIt;
    typename NodeContainer::ConstIterator pointsEnd;
  
    pointsIt = inputNarrowBand->Begin();
    pointsEnd = inputNarrowBand->End();

    double maxValue = narrowBandwidth / 2.0;
    NodeType node;
    PixelType lsetPixel;
    double curvature;
    double magnitude;
    double updateValue;
    double speed;
    double value;
    double deriv;

    for( ; pointsIt != pointsEnd; ++pointsIt )
      {
      node = pointsIt.Value();

      if( vnl_math_abs( node.GetValue() ) <= maxValue )
        {

        magnitude = inEntropy->EvaluateAtIndex( node.GetIndex() );
        updateValue = m_InflationStrength * magnitude;
        if( propagateOutwards )
          {
          updateValue *= -1.0;
          }

        curvature = inCurvature->EvaluateAtIndex( node.GetIndex() );
        magnitude = inCurvature->GetMagnitude();
        updateValue += curvature * magnitude;

        typedef typename TEdgeImage::PixelType EdgePixelType;
        speed = (double) edgeImage->GetPixel(node.GetIndex());

        updateValue *= speed;

        for( unsigned int j = 0; j < SetDimension; j++ )
          {
          typedef typename TDerivImage::PixelType DerivPixelType;
          deriv = (double) m_DerivImages[j]->GetPixel( node.GetIndex() );

          inUpwind->SetSpeed( -1.0 * deriv );
          
          updateValue += deriv * inUpwind->EvaluateAtIndex( node.GetIndex(), j );

          }

        updateValue *= timeStepSize; 
    
        value = (double) inputPtr->GetPixel( node.GetIndex() );
        value += updateValue;

        lsetPixel =  value;
        outputPtr->SetPixel( node.GetIndex(), lsetPixel );

        }

      } // end while loop

    } // end iteration loop

  this->SetOutputNarrowBand( inputNarrowBand );
  this->CopyOutputBufferToOutput();
                                          
} 


} // namespace itk

#endif
