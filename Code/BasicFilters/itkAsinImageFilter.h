/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkAsinImageFilter.h
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
#ifndef __itkAsinImageFilter_h
#define __itkAsinImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "vnl/vnl_math.h"

namespace itk
{
  
/** \class AsinImageFilter
 * \brief Computes the asin(x) pixel-wise
 *
 * This filter is templated over the pixel type of the input image
 * and the pixel type of the output image. 
 *
 * The filter will walk over all the pixels in the input image, and for
 * each one of them it will do the following: 
 *
 * - cast the pixel value to \c double, 
 * - apply the \c asin() function to the \c double value
 * - cast the \c double value resulting from \c asin() to the pixel type of the output image 
 * - store the casted value into the output image.
 * 
 * The filter expect both images to have the same dimension (e.g. both 2D, or both 3D, or both ND)
 * 
 * \ingroup IntensityImageFilters   Multithreaded
 *
 */

namespace Functor {  
  
  template< class TInput, class TOutput>
  class Asin
  {
  public:
    Asin() {};
    ~Asin() {};
    inline TOutput operator()( const TInput & A )
    {
      return static_cast<TOutput>(
          asin(
            static_cast<double>(A)
            )
          );
    }
  }; 

}
// Wrap: AsinImageFilter<$Image,$Image,$Image,$Function>
// Wrap: <XML code for Function....>
// Wrap: AsinImageFilter<Image<$BasicPixel,$BasicDimension>,$Image,$Image,$Function>
template <class TInputImage, class TOutputImage>
class ITK_EXPORT AsinImageFilter :
    public
    UnaryFunctorImageFilter<TInputImage,TOutputImage, 
    Functor::Asin< 
              typename TInputImage::PixelType, 
              typename TOutputImage::PixelType>   >


{
public:
  /**
   * Standard "Self" typedef.
   */
  typedef AsinImageFilter  Self;

  /**
   * Standard "Superclass" typedef.
   */
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage, 
    Functor::Asin< 
              typename TInputImage::PixelType, 
              typename TOutputImage::PixelType>   
                >  Superclass;

  /** 
   * Smart pointer typedef support 
   */
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;


  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);
  
protected:

  AsinImageFilter() {}
  virtual ~AsinImageFilter() {}
  AsinImageFilter(const Self&) {}
  void operator=(const Self&) {}


};

} // end namespace itk


#endif
