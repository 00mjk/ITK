/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkVectorCurvatureAnisotropicDiffusionImageFilter.h
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
#ifndef __itkVectorCurvatureAnisotropicDiffusionImageFilter_h_
#define __itkVectorCurvatureAnisotropicDiffusionImageFilter_h_

#include "itkExceptionObject.h"
#include "itkAnisotropicDiffusionImageFilter.h"
#include "itkVectorCurvatureNDAnisotropicDiffusionFunction.h"

namespace itk {

/** \class VectorCurvatureAnisotropicDiffusionImageFilter
 * 
 * \ingroup ImageEnhancement
 *
 *\todo Document.
 */
template <class TInputImage, class TOutputImage>
class VectorCurvatureAnisotropicDiffusionImageFilter
  : public AnisotropicDiffusionImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard itk typedefs */
  typedef VectorCurvatureAnisotropicDiffusionImageFilter Self;
  typedef AnisotropicDiffusionImageFilter<TInputImage, TOutputImage>
   Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Instantiation through object factory. */
  itkNewMacro(Self);

  /** Run-time type information. */
  itkTypeMacro(VectorCurvatureAnisotropicDiffusionImageFilter,
               AnisotropicDiffusionImageFilter);
  
  /** Convenient typedef. */
  typedef typename Superclass::UpdateBufferType UpdateBufferType;

  /** Determine the image dimension. */
  enum { ImageDimension = Superclass::ImageDimension };
  
protected:
  VectorCurvatureAnisotropicDiffusionImageFilter()
    {
      VectorCurvatureNDAnisotropicDiffusionFunction<UpdateBufferType>::Pointer q
        = VectorCurvatureNDAnisotropicDiffusionFunction<UpdateBufferType>::New();
      this->SetDifferenceFunction(q);
    }
  ~VectorCurvatureAnisotropicDiffusionImageFilter() {}

private:
  VectorCurvatureAnisotropicDiffusionImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namspace itk

#endif
