/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkNaryFunctorImageFilter.h
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
#ifndef __itkNaryFunctorImageFilter_h
#define __itkNaryFunctorImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImageIterator.h"

namespace itk
{
  
/** \class NaryFunctorImageFilter
 * \brief Implements pixel-wise generic operation of Nth similar images.
 *
 * This class is parameterized over the types of the input images
 * and the type of the output image.  It is also parameterized by the
 * operation to be applied.  A Functor style is used to represent the
 * function.
 *
 * All the input images are of the same type.
 * 
 * \ingroup IntensityImageFilters   Multithreaded
 * */

template <class TInputImage, class TOutputImage, class TFunction >
class ITK_EXPORT NaryFunctorImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage> 

{
public:
  /**
   * Standard "Self" typedef.
   */
  typedef NaryFunctorImageFilter  Self;

  /**
   * Standard "Superclass" typedef.
   */
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;

  /** 
   * Smart pointer typedef support 
   */
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);
  
  /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro(NaryFunctorImageFilter, ImageToImageFilter);

  /** 
   * Some typedefs.
   */
  typedef TFunction   FunctorType;

  typedef TInputImage InputImageType;
  typedef typename InputImageType::Pointer      InputImagePointer;
  typedef typename InputImageType::RegionType   InputImageRegionType; 
  typedef typename InputImageType::PixelType    InputImagePixelType; 
  typedef TOutputImage OutputImageType;
  typedef typename OutputImageType::Pointer     OutputImagePointer;
  typedef typename OutputImageType::RegionType  OutputImageRegionType;
  typedef typename OutputImageType::PixelType   OutputImagePixelType;

  /**
   * Connect one of the operands for pixel-wise operation
   */
   void SetInput( unsigned int, TInputImage * image);

  /**
   * Get the functor object.  The functor is returned by reference.
   * (Functors do not have to derive from itk::LightObject, so they do
   * not necessarily have a reference count. So we cannot return a
   * SmartPointer.)
   */
  FunctorType& GetFunctor() { return m_Functor; };

  /**
   * Set the functor object.  This replaces the current Functor with a
   * copy of the specified Functor. This allows the user to specify a
   * functor that has ivars set differently than the default functor.
   * This method requires an operator!=() be defined on the functor
   * (or the compiler's default implementation of operator!=() being
   * appropriate).
   */
  void SetFunctor(FunctorType& functor)
  {
    if ( m_Functor != functor )
      {
      m_Functor = functor;
      this->Modified();
      }
  }
  
protected:
  NaryFunctorImageFilter();
  virtual ~NaryFunctorImageFilter() {};
  NaryFunctorImageFilter(const Self&) {}
  void operator=(const Self&) {}

  /**
   * NaryFunctorImageFilter can be implemented as a multithreaded filter.
   * Therefore, this implementation provides a ThreadedGenerateData() routine
   * which is called for each processing thread. The output image data is
   * allocated automatically by the superclass prior to calling
   * ThreadedGenerateData().  ThreadedGenerateData can only write to the
   * portion of the output image specified by the parameter
   * "outputRegionForThread"
   *
   * \sa ImageToImageFilter::ThreadedGenerateData(),
   *     ImageToImageFilter::GenerateData() 
   */
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            int threadId );

 private:
  FunctorType m_Functor;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNaryFunctorImageFilter.txx"
#endif

#endif
