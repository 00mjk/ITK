/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBloxPixel.h
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
#ifndef __itkBloxPixel_h
#define __itkBloxPixel_h

#include <list>

#include "itkBloxItem.h"

namespace itk
{

/**
 * \class BloxPixel
 * \brief Holds a linked list of BloxItem's
 *
 * itk::BloxPixel is a specialized "value added" version of the basic STL list
 * intended as a base class for all pixels stored in itk::BloxImage derived classes.
 * A particular type of itk::BloxImage is fully specialized by setting the type
 * of itk::BloxPixel that it holds, so in some sense this is the most important
 * class in the blox hierarchy.
 *
 * It is assumed that particular itk::BloxPixel derived types will add functionality
 * to this base class; for example, eigenanalysis of core atom populations in
 * itk::BloxCoreAtomPixel
 *
 * \ingroup ImageObjects
 * */

template <typename TItemType>
class BloxPixel : public std::list<TItemType*>
{
public:
  BloxPixel();
  ~BloxPixel();
};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBloxPixel.txx"
#endif

#endif
