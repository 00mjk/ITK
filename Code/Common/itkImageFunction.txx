/*==========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageFunction.txx
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
#ifndef _itkImageFunction_txx
#define _itkImageFunction_txx

#include "itkImageFunction.h"
#include "itkOffset.h"
#include "vnl/vnl_math.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputImage, class TOutput>
ImageFunction<TInputImage, TOutput>
::ImageFunction()
{
  m_Image = NULL;
}


/**
 * Standard "PrintSelf" method
 */
template <class TInputImage, class TOutput>
void
ImageFunction<TInputImage, TOutput>
::PrintSelf(
std::ostream& os, 
Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "InputImage: " << m_Image.GetPointer() << std::endl;

}


/**
 * Initialize by setting the input image
 */
template <class TInputImage, class TOutput>
void
ImageFunction<TInputImage, TOutput>
::SetInputImage(
const InputImageType * ptr )
{

  // set the input image
  m_Image = ptr;

  if( m_Image )
    {
    m_Origin = m_Image->GetOrigin();
    m_Spacing = m_Image->GetSpacing();

    const typename InputImageType::SizeType & size =
      m_Image->GetBufferedRegion().GetSize();

    Offset<ImageDimension> offset;
    offset.Fill( -1 );
   
    m_BufferStart = m_Image->GetBufferedRegion().GetIndex();
    m_BufferEnd = m_BufferStart + size + offset;

    this->ConvertIndexToPoint( m_BufferStart, m_GeometricStart );
    this->ConvertIndexToPoint( m_BufferEnd, m_GeometricEnd );

    }
    
}


/**
 * Check if an index is inside the image buffer
 */
template <class TInputImage, class TOutput>
bool
ImageFunction<TInputImage, TOutput>
::IsInsideBuffer(
const IndexType& index ) const
{
  for( int j = 0; j < ImageDimension; j++ )
    {
    if( index[j] < m_BufferStart[j] ||
        index[j] > m_BufferEnd[j] )
      {
      return false;
      }
    }
  return true;
}


/**
 * Check if a continuous index is inside the image buffer
 */
template <class TInputImage, class TOutput>
bool
ImageFunction<TInputImage, TOutput>
::IsInsideBuffer(
const ContinuousIndexType& index ) const
{
  for( int j = 0; j < ImageDimension; j++ )
    {
    if( index[j] < (double) m_BufferStart[j] ||
        index[j] > (double) m_BufferEnd[j] )
      {
      return false;
      }
    }
  return true;
}


/**
 * Check if a point is inside the image buffer
 */
template <class TInputImage, class TOutput>
bool
ImageFunction<TInputImage, TOutput>
::IsInsideBuffer(
const PointType& point ) const
{
  for( int j = 0; j < ImageDimension; j++ )
    {
    if( point[j] < m_GeometricStart[j] ||
        point[j] > m_GeometricEnd[j] )
      {
      return false;
      }
    }
  return true;
}


/**
 * Convert a point to a continuous index
 */
template <class TInputImage, class TOutput>
void
ImageFunction<TInputImage, TOutput>
::ConvertPointToContinuousIndex(
const PointType& point,
ContinuousIndexType& index ) const
{
  for( int j = 0; j < ImageDimension; j++ )
    {
    index[j] = ( point[j] - m_Origin[j] ) / m_Spacing[j];
    }
}


/**
 * Convert a continuous index to a point
 */
template <class TInputImage, class TOutput>
void
ImageFunction<TInputImage, TOutput>
::ConvertContinuousIndexToPoint(
const ContinuousIndexType& index,
PointType& point ) const
{
  for( int j = 0; j < ImageDimension; j++ )
    {
    point[j] = index[j] * m_Spacing[j] + m_Origin[j];
    }
}


/**
 * Convert a index to a point
 */
template <class TInputImage, class TOutput>
void
ImageFunction<TInputImage, TOutput>
::ConvertIndexToPoint(
const IndexType& index,
PointType& point ) const
{
  for( int j = 0; j < ImageDimension; j++ )
    {
    point[j] = (double) index[j] * m_Spacing[j] + m_Origin[j];
    }
}


/**
 * Convert a point to a nearest index
 */
template <class TInputImage, class TOutput>
void
ImageFunction<TInputImage, TOutput>
::ConvertPointToNearestIndex(
const PointType& point,
IndexType& index ) const
{
  for( int j = 0; j < ImageDimension; j++ )
    {
    index[j] = vnl_math_rnd(( point[j] - m_Origin[j] ) / m_Spacing[j]);
    }
}


/**
 * Convert a continuous index to a nearest index
 */
template <class TInputImage, class TOutput>
void
ImageFunction<TInputImage, TOutput>
::ConvertContinuousIndexToNearestIndex(
const ContinuousIndexType& cindex,
IndexType& index ) const
{
  for( int j = 0; j < ImageDimension; j++ )
    {
    index[j] = vnl_math_rnd( cindex[j] );
    }
}


} // end namespace itk

#endif

