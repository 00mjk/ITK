/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBloxBoundaryPointImage.txx
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
#ifndef __itkBloxBoundaryPointImage_txx
#define __itkBloxBoundaryPointImage_txx

#include <iostream>
#include "itkImageRegionIterator.h"
#include "itkBloxBoundaryPointImage.h"

namespace itk
{

template<class TSourceImage, class TImageTraits>
BloxBoundaryPointImage<TSourceImage, TImageTraits>
::BloxBoundaryPointImage()
{
  m_Threshold = 0;
}

template<class TSourceImage, class TImageTraits>
BloxBoundaryPointImage<TSourceImage, TImageTraits>
::~BloxBoundaryPointImage()
{

}

template<class TSourceImage, class TImageTraits>
void
BloxBoundaryPointImage<TSourceImage, TImageTraits>
::UpdateSourceParameters()
{

  m_SourceOrigin = m_SourceImage->GetOrigin();
  m_SourceSpacing = m_SourceImage->GetSpacing();

  std::cout << "m_SourceOrigin = " << m_SourceOrigin[0] << "," << m_SourceOrigin[1] << ","
    << m_SourceOrigin[2] << "\n";

  std::cout << "m_SourceSpacing = " << m_SourceSpacing[0] << "," << m_SourceSpacing[1] << ","
    << m_SourceSpacing[2] << "\n";

}

template<class TSourceImage, class TImageTraits>
void
BloxBoundaryPointImage<TSourceImage, TImageTraits>
::FindBoundaryPoints()
{
  std::cout << "BloxBoundaryPointImage::FindBoundaryPoints() called\n";

  // Update origin and spacing of the source image
  this->UpdateSourceParameters();

  // Make sure we're getting everything
  m_SourceImage->SetRequestedRegionToLargestPossibleRegion();

  // Position to figure out pixel location
  TPositionType sourcePosition;

  // Create an iterator to walk the source image
  typedef ImageRegionIterator<TSourceImage> sourceIterator;

  sourceIterator sourceIt = sourceIterator(m_SourceImage,
                                           m_SourceImage->GetRequestedRegion() );

  // Keep track of how many boundary points we found (for debugging)
  unsigned long int numBP = 0;
  unsigned long int numBPadded = 0;

  // Get the index of the pixel
  typename TSourceImage::IndexType sourceIndex;
  IndexType bloxIndex;
  
  for ( sourceIt.GoToBegin(); !sourceIt.IsAtEnd(); ++sourceIt)
    {
    // Figure out the magnitude of the gradient
    double mag = 0;

    for(int i = 0; i < NDimensions; i++)
      {
      mag += sourceIt.Get()[i]
        * sourceIt.Get()[i];
      }

    mag = sqrt(mag);

    // If the pixel meets threshold requirements, add it to the image
    if( mag >= m_Threshold)
      {
      numBP++;

      // Get the index of the boundary pixel
      sourceIndex = sourceIt.GetIndex();

      // Convert the index of the source pixel to the physical location of the
      // boundary point in the source image
      m_SourceImage->TransformIndexToPhysicalPoint(sourceIndex, sourcePosition);

      // Transform the physical location to a blox index
      this->TransformPhysicalPointToIndex(sourcePosition, bloxIndex);

      // Create a new boundary point item and set its parameters
      BloxBoundaryPointItem<NDimensions>* pItem = new BloxBoundaryPointItem<NDimensions>;
      pItem->SetPhysicalPosition(sourcePosition);
      pItem->SetGradient( sourceIt.Get() );

      this->GetPixel(bloxIndex).push_back(pItem);
      numBPadded++;
      }
    }
 
  std::cout << "Finished looking for boundary points\n";
  std::cout << "I found " << numBP << " points\n";
  std::cout << "I added " << numBPadded << " points\n";
}

} // end namespace itk

#endif
