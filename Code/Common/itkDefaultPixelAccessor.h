/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkDefaultPixelAccessor.h
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
#ifndef __itkDefaultPixelAccessor_h
#define __itkDefaultPixelAccessor_h

#include "itkMacro.h"

namespace itk
{

/**
 * \class DefaultPixelAccessor
 * \brief Give access to partial aspects a type
 *
 * DefaultPixelAccessor is templated over an internal type and an
 * external type representation. This class encapsulates a
 * customized convertion between the internal and external
 * type representations.
 *
 * PixelAccessor is the class responsible for pixel-to-pixel
 * transformation during image data access. The DefaultPixelAccessor
 * is an identity operation on the pixel value. It only exist in
 * order to standarize the way in which pixels are accessed.
 *
 * PixelAccessor are used by ImageAdaptors in order to present
 * an Image as being of a different type. The usual application
 * of ImageAccessors is Image casting, by avoiding the overhead
 * of an ImageFilter that performs the complete transformation
 *
 * \sa ImageAdaptor
 * \sa PixelAccessor
 * \sa Image
 *
 * \ingroup ImageAdaptors
 *
 */

template <class TType>
class ITK_EXPORT DefaultPixelAccessor  
{
public:

 /** External typedef. It defines the external aspect
   * that this class will exhibit. */
  typedef TType ExternalType ;

  /** Internal typedef. It defines the internal real
   * representation of data. */
  typedef TType InternalType ;

  /** Set the pixel. */
  inline void Set(TType & output, const TType & input) const
    {output = input;}

  /** Get the pixel. */
  inline TType & Get( TType & input ) const
    {return input;}

  /** Get a const reference to the pixel. */
  inline const TType & Get( const TType & input ) const
    {return input;}
  
};

  
} // end namespace itk
  

#endif

