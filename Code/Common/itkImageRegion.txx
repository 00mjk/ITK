/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageRegion.txx
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
#ifndef _itkImageRegion_txx
#define _itkImageRegion_txx
#include "itkImageRegion.h"

namespace itk
{

/**
 * Instantiate object.
 */
template<unsigned int VImageDimension>
ImageRegion<VImageDimension>
::ImageRegion()
{
  for(unsigned int i=0; i<VImageDimension; i++)
  {
    m_Index[i] = 0; 
    m_Size[i]  = 0;
  }
}

/**
 * Destructor for the ImageRegion class.
 */
template<unsigned int VImageDimension>
ImageRegion<VImageDimension>
::~ImageRegion()
{
}

template<unsigned int VImageDimension>
unsigned long 
ImageRegion<VImageDimension>
::GetNumberOfPixels() const
{
  unsigned long numPixels=1;

  for (unsigned int i=0; i<VImageDimension; i++)
    {
    numPixels *= m_Size[i];
    }
  
  return numPixels;
}

template<unsigned int VImageDimension>
void
ImageRegion<VImageDimension>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  
  os << indent << "Dimension: " << this->GetImageDimension() << std::endl;
  os << indent << "Index: " << this->GetIndex() << std::endl;
  os << indent << "Size: " << this->GetSize() << std::endl;
}


template<unsigned int VImageDimension>
void
ImageRegion<VImageDimension>
::PadByRadius(unsigned long radius)
{
  unsigned long radiusVector[VImageDimension];

  for (unsigned int i=0; i < VImageDimension; ++i)
    {
      radiusVector[i] = radius;
    }

  this->PadByRadius(radiusVector);
}

template<unsigned int VImageDimension>
void
ImageRegion<VImageDimension>
::PadByRadius(const SizeType &radius)
{
  this->PadByRadius( radius.GetSize() );
}

template<unsigned int VImageDimension>
void
ImageRegion<VImageDimension>
::PadByRadius(const unsigned long radius[VImageDimension])
{
  for (unsigned int i = 0; i < VImageDimension; i++)
    {
    m_Size[i] += 2 * radius[i];
    m_Index[i] -= static_cast<long>(radius[i]);
    }  
}

template<unsigned int VImageDimension>
bool
ImageRegion<VImageDimension>
::Crop(const Self& region)
{
  long crop=0;
  unsigned int i;
  bool cropPossible = true;

  // Can we crop?
  for (i = 0; i < VImageDimension && cropPossible; i++)
    {
    // Is left edge of current region to the right of the right edge
    // of the region to crop with? (if so, we cannot crop)
    if (m_Index[i] >= region.GetIndex()[i]
        + static_cast<long>(region.GetSize()[i]))
      {
      cropPossible = false;
      }
    // If right edge of the current region to the left of the left
    // edge of the region to crop with? (if so, we cannot crop)
    if (m_Index[i] + static_cast<long>(m_Size[i]) <= region.GetIndex()[i])
      {
      cropPossible = false;
      }
    }

  // if we cannot crop, return without changing anythin
  if (!cropPossible)
    {
    return cropPossible;
    }

  // we can crop, so crop
  for (i=0; i < VImageDimension; i++)
    {
    // first check the start index
    if (m_Index[i] < region.GetIndex()[i])
      {
      // how much do we need to adjust
      crop = region.GetIndex()[i] - m_Index[i];

      // adjust the start index and the size of the current region
      m_Index[i] += crop;
      m_Size[i] -= static_cast<unsigned long>(crop);
      }
    // now check the final size
    if (m_Index[i] + static_cast<long>(m_Size[i])
        > region.GetIndex()[i] + static_cast<long>(region.GetSize()[i]))
      {
      // how much do we need to adjust
      crop = m_Index[i] + static_cast<long>(m_Size[i])
        - region.GetIndex()[i] - static_cast<long>(region.GetSize()[i]);

      // adjust the size
      m_Size[i] -= static_cast<unsigned long>(crop);
      }
    }

  return cropPossible;
}


} // end namespace itk
#endif
