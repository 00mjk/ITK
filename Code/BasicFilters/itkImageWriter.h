/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageWriter.h
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
#ifndef __itkImageWriter_h
#define __itkImageWriter_h

#include "itkWriter.h"

namespace itk
{

/** \class ImageWriter
 * \brief Base class for all writers that write images.
 *
 * ImageWriter is the base class for writers that write images.
 * \ingroup IOFilters
 */
template <class TInputImage>
class ITK_EXPORT ImageWriter : public Writer
{
public:
  /**
   * Standard "Self" typedef.
   */
  typedef ImageWriter          Self;

  /**
   * Standard "Superclass" typedef.
   */
  typedef Writer   Superclass;

  /** 
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** 
   * Some typedefs
   */
  typedef TInputImage InputImageType;
  typedef typename InputImageType::Pointer InputImagePointer;

  /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro(ImageWriter,Writer);

  /** 
   * Set the input image of this writer. 
   */
  void SetInput(TInputImage *input);

  /** 
   * Get the input image of this writer.
   */
  InputImagePointer GetInput();

protected:
  ImageWriter() {}
  ~ImageWriter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  ImageWriter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageWriter.txx"
#endif

#endif
  
