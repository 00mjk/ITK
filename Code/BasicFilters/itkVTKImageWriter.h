/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkVTKImageWriter.h
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
#ifndef __itkVTKImageWriter_h
#define __itkVTKImageWriter_h

#include "itkImageWriter.h"
#include <vector>

namespace itk
{

/** \class VTKImageWriter
 * \brief Write an image (dimension 1-3D) in VTK format.
 *
 * VTKImageWriter writes 1-3D images in VTK file format. You can specify
 * binary or ASCII output types. The class is limited in the type of
 * scalars that are supported: all native types with up to four components.
 *
 * \ingroup IOFilters
 */
template <class TInputImage>
class ITK_EXPORT VTKImageWriter : public ImageWriter<TInputImage>
{
public:
  /**
   * Standard "Self" typedef.
   */
  typedef VTKImageWriter       Self;

  /**
   * Standard "Superclass" typedef.
   */
  typedef ImageWriter<TInputImage>  Superclass;

  /** 
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro(VTKImageWriter,ImageWriter);

  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);
  
protected:
  VTKImageWriter();
  ~VTKImageWriter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  void WriteData();
  
private:
  VTKImageWriter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  bool               m_WriteToOutputString;
  std::vector<char>  m_OutputBuffer;
  
  std::ostream *OpenVTKFile();
  bool WriteVTKHeader(std::ostream *fp);
  bool VTKImageWriterData(std::ostream *fp, TInputImage *input);
  void CloseVTKFile(std::ostream *fp);  
};

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVTKImageWriter.txx"
#endif

#endif
