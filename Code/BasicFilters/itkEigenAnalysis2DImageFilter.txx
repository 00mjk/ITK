/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkEigenAnalysis2DImageFilter.txx
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
#ifndef _itkEigenAnalysis2DImageFilter_txx
#define _itkEigenAnalysis2DImageFilter_txx

#include "itkEigenAnalysis2DImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"


namespace itk
{


/**
 * Constructor
 */
template <class TInputImage, class TEigenValueImage, class TEigenVectorImage>
EigenAnalysis2DImageFilter<TInputImage,TEigenValueImage,TEigenVectorImage>
::EigenAnalysis2DImageFilter()
{
  this->SetNumberOfRequiredInputs( 3 );
  this->SetNumberOfRequiredOutputs( 3 );
  this->SetNthOutput( 0, this->MakeOutput( 0 ) );
  this->SetNthOutput( 1, this->MakeOutput( 1 ) );
  this->SetNthOutput( 2, this->MakeOutput( 2 ) );
  
}



/**
 * Connect one the image containing the [0,0] elements of the input matrix
 */
template <class TInputImage, class TEigenValueImage, class TEigenVectorImage> 
void
EigenAnalysis2DImageFilter<TInputImage,TEigenValueImage,TEigenVectorImage>
::SetInput1( TInputImage * image ) 
{
  this->SetNthInput(0, image);
}


/**
 * Connect one the image containing the [0,1] elements of the input matrix
 * this element is the same [1,0] because this filter assumes symmetric 
 * matrices
 */
template <class TInputImage, class TEigenValueImage, class TEigenVectorImage> 
void
EigenAnalysis2DImageFilter<TInputImage,TEigenValueImage,TEigenVectorImage>
::SetInput2( TInputImage * image ) 
{
  this->SetNthInput(1, image);
}


/**
 * Connect one the image containing the [1,1] elements of the input matrix
 */
template <class TInputImage, class TEigenValueImage, class TEigenVectorImage> 
void
EigenAnalysis2DImageFilter<TInputImage,TEigenValueImage,TEigenVectorImage>
::SetInput3( TInputImage * image ) 
{
  this->SetNthInput(2, image);
}




/**
 * Get the greatest eigenvalue considering the sign
 */
template <class TInputImage, class TEigenValueImage, class TEigenVectorImage> 
EigenAnalysis2DImageFilter<TInputImage,TEigenValueImage,TEigenVectorImage>::EigenValueImagePointer
EigenAnalysis2DImageFilter<TInputImage,TEigenValueImage,TEigenVectorImage>
::GetMaxEigenValue( void )
{
  return dynamic_cast<EigenValueImageType *>(
                    this->GetOutput( 0 ).GetPointer() );
}




/**
 * Get the smallest eigenvalue considering the sign
 */
template <class TInputImage, class TEigenValueImage, class TEigenVectorImage>
EigenAnalysis2DImageFilter<TInputImage,TEigenValueImage,TEigenVectorImage>::EigenValueImagePointer
EigenAnalysis2DImageFilter<TInputImage,TEigenValueImage,TEigenVectorImage>
::GetMinEigenValue( void )
{
  return dynamic_cast<EigenValueImageType *>(
                    this->GetOutput( 1 ).GetPointer() );
}



/**
 * Get the smallest eigenvalue considering the sign
 */
template <class TInputImage, class TEigenValueImage, class TEigenVectorImage> 
EigenAnalysis2DImageFilter<TInputImage,TEigenValueImage,TEigenVectorImage>::EigenVectorImagePointer
EigenAnalysis2DImageFilter<TInputImage,TEigenValueImage,TEigenVectorImage>
::GetMaxEigenVector( void )
{
  return dynamic_cast<EigenVectorImageType *>(
                    this->GetOutput( 2 ).GetPointer() );
}



/**
 *   Make Ouput
 */
template <class TInputImage, class TEigenValueImage, class TEigenVectorImage> 
DataObject::Pointer
EigenAnalysis2DImageFilter<TInputImage,TEigenValueImage,TEigenVectorImage>
::MakeOutput(unsigned int idx)
{
  DataObject::Pointer output;
  switch( idx )
  {
    case 0:
      output = (EigenValueImageType::New()).GetPointer();
      break;
    case 1:
      output = (EigenValueImageType::New()).GetPointer();
      break;
    case 2:
      output = (EigenVectorImageType::New()).GetPointer();
      break;
  }
  return output.GetPointer();
}




template <class TInputImage, class TEigenValueImage, class TEigenVectorImage>
void
EigenAnalysis2DImageFilter<TInputImage,TEigenValueImage,TEigenVectorImage>
::GenerateData()
{

  typename TInputImage::Pointer inputPtr1(
                     dynamic_cast<TInputImage *>(
                           (ProcessObject::GetInput(0)).GetPointer()));

  typename TInputImage::Pointer inputPtr2(
                     dynamic_cast<TInputImage *>(
                           (ProcessObject::GetInput(1)).GetPointer()));

  typename TInputImage::Pointer inputPtr3(
                     dynamic_cast<TInputImage *>(
                           (ProcessObject::GetInput(2)).GetPointer()));

  EigenValueImagePointer   outputPtr1 = this->GetMaxEigenValue();
  EigenValueImagePointer   outputPtr2 = this->GetMinEigenValue();
  EigenVectorImagePointer  outputPtr3 = this->GetMaxEigenVector();

  outputPtr1->SetLargestPossibleRegion( inputPtr1->GetLargestPossibleRegion() );
  outputPtr2->SetLargestPossibleRegion( inputPtr1->GetLargestPossibleRegion() );
  outputPtr3->SetLargestPossibleRegion( inputPtr1->GetLargestPossibleRegion() );
      
  outputPtr1->SetBufferedRegion( inputPtr1->GetBufferedRegion() );
  outputPtr2->SetBufferedRegion( inputPtr1->GetBufferedRegion() );
  outputPtr3->SetBufferedRegion( inputPtr1->GetBufferedRegion() );
      
  outputPtr1->SetRequestedRegion( inputPtr1->GetRequestedRegion() );
  outputPtr2->SetRequestedRegion( inputPtr1->GetRequestedRegion() );
  outputPtr3->SetRequestedRegion( inputPtr1->GetRequestedRegion() );
      
  outputPtr1->Allocate();
  outputPtr2->Allocate();
  outputPtr3->Allocate();

  EigenValueImageRegionType  region = outputPtr1->GetRequestedRegion();

  ImageRegionIteratorWithIndex<TInputImage>   inputIt1( inputPtr1, region );
  ImageRegionIteratorWithIndex<TInputImage>   inputIt2( inputPtr2, region );
  ImageRegionIteratorWithIndex<TInputImage>   inputIt3( inputPtr3, region );

  ImageRegionIteratorWithIndex<EigenValueImageType>   outputIt1(outputPtr1, region );
  ImageRegionIteratorWithIndex<EigenValueImageType>   outputIt2(outputPtr2, region );
  ImageRegionIteratorWithIndex<EigenVectorImageType>  outputIt3(outputPtr3, region );

  EigenVectorType nullVector;
  nullVector.Fill( 0.0 );

  // support progress methods/callbacks
  unsigned long updateVisits = 0, i=0;
  updateVisits = region.GetNumberOfPixels()/10;
  if ( updateVisits < 1 ) 
    {
    updateVisits = 1;
    }
        
  inputIt1.GoToBegin();
  inputIt2.GoToBegin();
  inputIt3.GoToBegin();
  
  outputIt1.GoToBegin();
  outputIt2.GoToBegin();
  outputIt3.GoToBegin();

  EigenVectorType eigenVector;

  i = 0;
  while( !inputIt1.IsAtEnd() ) 
    {
    if ( !(i % updateVisits ) )
      {
      this->UpdateProgress((float)i/(float(updateVisits)*10.0));
      }

    const double xx = static_cast<double>( inputIt1.Get() );
    const double xy = static_cast<double>( inputIt2.Get() );
    const double yy = static_cast<double>( inputIt3.Get() );

    const double dxy = xx - yy;
    const double sxy = xx + yy;

    const double S = sqrt( dxy * dxy + 4.0 * xy * xy);

    const double pp =  ( sxy + S ) / 2.0;
    const double qq =  ( sxy - S ) / 2.0;

    outputIt1.Set( pp );
    outputIt2.Set( qq );

    eigenVector[0] = static_cast<VectorComponentType>(( -dxy - S ) / 2.0 );
    eigenVector[1] = static_cast<VectorComponentType>( -xy );


    outputIt3.Set( eigenVector );

    const VectorComponentType norm = eigenVector.GetNorm();
		if( norm > 1e-30 ) 
      {
      outputIt3.Set( eigenVector / norm  );
			}
		else
      {
      outputIt3.Set( nullVector );
      }

    ++inputIt1;
    ++inputIt2;
    ++inputIt3;

    ++outputIt1;
    ++outputIt2;
    ++outputIt3;
    
    ++i;

    }
}
} // end namespace itk

#endif
