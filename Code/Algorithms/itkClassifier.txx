/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkClassifier.txx
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
#ifndef _itkClassifier_txx
#define _itkClassifier_txx

#include "itkCommand.h"

namespace itk
{

template<class TInputImage, class TClassifiedImage>
Classifier<TInputImage,TClassifiedImage>
::Classifier(void)
{
  m_InputImage      = NULL;
  m_ClassifiedImage = NULL;
  m_NumberOfClasses  = 0;
}

template<class TInputImage,  class TClassifiedImage>
Classifier<TInputImage,TClassifiedImage>
::~Classifier()
{

}

/**
 * PrintSelf
 */
template <class TInputImage,  class TClassifiedImage>
void
Classifier<TInputImage,TClassifiedImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Classifier object" << std::endl;
  os << indent << "Pointer to the classified image: " << m_ClassifiedImage.GetPointer() << std::endl;
  os << indent << "Pointer to the input image     : " << m_InputImage.GetPointer() << std::endl;
  os << indent << "Progress value                 : " << m_Progress << std::endl;
  os << indent << "Number of classes              : " << m_NumberOfClasses << std::endl;

}// end PrintSelf


/**
 * Update the progress. If a ProgressMethod exists execute it.
 */
template <class TInputImage, class TClassifiedImage>
void
Classifier<TInputImage,TClassifiedImage>
::UpdateProgress( float amount )
{
  this->SetProgress( amount );
  this->InvokeEvent( ProgressEvent() );
  
}


} // namespace itk






















#endif
