/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLogImageAdaptor.h
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
#ifndef __itkLogImageAdaptor_h
#define __itkLogImageAdaptor_h

#include <itkImageAdaptor.h>
#include "vnl/vnl_math.h"

namespace itk
{
 
namespace Accessor {
/** \class LogPixelAccessor
 * \brief Give access to the log() function of a value
 *
 * LogPixelAccessor is templated over an internal type and an
 * external type representation. This class cast the input
 * applies the funtion to it and cast the result according 
 * to the types defined as template parameters
 *
 * \ingroup ImageAdaptors
 */

template <class TInternalType, class TExternalType >
class ITK_EXPORT LogPixelAccessor  
{
public:
 /** External typedef. It defines the external aspect
   * that this class will exhibit. */
  typedef TExternalType ExternalType;

  /** Internal typedef. It defines the internal real
   * representation of data. */
  typedef TInternalType InternalType;

  static inline void Set(TInternalType & output, const TExternalType & input) 
    {output = (TInternalType)log((double)input);}

  static inline TExternalType Get( const TInternalType & input ) 
    {return (TExternalType)log((double)input);}

};

  
} // end namespace Accessor

/** \class LogImageAdaptor
 * \brief Presents an image as being composed of the log() of its pixels
 *
 * Additional casting is performed according to the input and output image
 * types following C++ default casting rules.
 *
 * \ingroup ImageAdaptors
 */
template <class TImage, class TOutputPixelType>
class ITK_EXPORT LogImageAdaptor : public
      ImageAdaptor<TImage,
                   Accessor::LogPixelAccessor<
                                      typename TImage::PixelType,
                                      TOutputPixelType>   >
{
public:
  /** Standard class typedefs. */
  typedef LogImageAdaptor  Self;
  typedef ImageAdaptor<TImage,
                       Accessor::LogPixelAccessor<
                                 typename TImage::PixelType,
                                 TOutputPixelType> >  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro( LogImageAdaptor, ImageAdaptor );

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

protected:
  LogImageAdaptor() {}
  virtual ~LogImageAdaptor() {}
  
private:
  LogImageAdaptor(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk

#endif
