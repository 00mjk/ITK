/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCurvatureFlowImageFilter.txx
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
#ifndef __itkCurvatureFlowImageFilter_txx_
#define __itkCurvatureFlowImageFilter_txx_

#include "itkExceptionObject.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputImage, class TOutputImage>
CurvatureFlowImageFilter<TInputImage, TOutputImage>
::CurvatureFlowImageFilter()
{

  m_NumberOfIterations = 0;
  m_TimeStep   = 0.125f;

  typename CurvatureFlowFunctionType::Pointer cffp;
  cffp = CurvatureFlowFunctionType::New();

  this->SetDifferenceFunction( static_cast<FiniteDifferenceFunctionType *>( 
    cffp.GetPointer() ) );

}


/**
 * Standard PrintSelf method.
 */
template <class TInputImage, class TOutputImage>
void
CurvatureFlowImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "CurvatureFlowImageFilter";
  os << indent << "Number of Iterations: " << m_NumberOfIterations;
  os << indent << "Time step: " << m_TimeStep;
}


/**
 * Initialize the state of filter and equation before each iteration.
 */
template <class TInputImage, class TOutputImage>
void
CurvatureFlowImageFilter<TInputImage, TOutputImage>
::InitializeIteration()
{

  // update variables in the equation object
  try
    {
    CurvatureFlowFunctionType *f = 
      dynamic_cast<CurvatureFlowFunctionType *>
      (this->GetDifferenceFunction().GetPointer());
    f->SetTimeStep( m_TimeStep );
    this->Superclass::InitializeIteration();           
    }
  catch( ... )
    {
    itkErrorMacro(<<"DifferenceFunction not of type CurvatureFlowFunction");
    throw ExceptionObject( __FILE__, __LINE__ );
    }

  // progress feedback
  if( m_NumberOfIterations <= 0 )
    {
    this->UpdateProgress( 1.0 );
    }
  else if ( m_NumberOfIterations < 100 || !(this->GetElapsedIterations() % 10) )
    {
    this->UpdateProgress( (float) this->GetElapsedIterations() /
                          (float) m_NumberOfIterations );
    } 
  
}


/**
 * GenerateInputRequestedRegion
 */
template <class TInputImage, class TOutputImage>
void
CurvatureFlowImageFilter<TInputImage, TOutputImage>
::GenerateInputRequestedRegion()
{
  // call the superclass's implementation
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the input and output
  InputImagePointer  inputPtr  = this->GetInput();
  OutputImagePointer outputPtr = this->GetOutput();

  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  // set the input requested region to be the same as
  // the output requested region
  inputPtr->SetRequestedRegion(
    outputPtr->GetRequestedRegion() );

}


/**
 * EnlargeOutputRequestedRegion
 */
template <class TInputImage, class TOutputImage>
void
CurvatureFlowImageFilter<TInputImage, TOutputImage>
::EnlargeOutputRequestedRegion(
DataObject * ptr )
{

  // convert DataObject pointer to OutputImageType pointer 
  OutputImageType * outputPtr;
  outputPtr = dynamic_cast<OutputImageType*>( ptr );

  // get input image pointer
  InputImagePointer  inputPtr  = this->GetInput();
  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  // Get the size of the neighborhood on which we are going to operate.  This
  // radius is supplied by the difference function we are using.
  typename FiniteDifferenceFunctionType::RadiusType radius
    = this->GetDifferenceFunction()->GetRadius();

  for( int j = 0; j < ImageDimension; j++ )
    {
    radius[j] *= m_NumberOfIterations;
    }

  /**
   * NewOutputRequestedRegion = OldOutputRequestedRegion +
   * radius * m_Iterations padding on each edge
   */
  typename OutputImageType::RegionType outputRequestedRegion =
    outputPtr->GetRequestedRegion();

  outputRequestedRegion.PadByRadius( radius );
  outputRequestedRegion.Crop( outputPtr->GetLargestPossibleRegion() );
  
  outputPtr->SetRequestedRegion( outputRequestedRegion );

  
}


} // end namespace itk

#endif
