/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkHybridFilter.h
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
#ifndef __itkHybridFilter_h
#define __itkHybridFilter_h

#include "itkBalloonForceFilter.h"
#include "itkGibbsPriorFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace itk
{

/** \class HybridFilter
 * \brief Computes the gradient of an image by convolution
 *        with the first derivative of a Gaussian.
 * 
 * This filter is implemented using the recursive gaussian
 * filters
 *
 * \ingroup HybridSegmentation 
 *
 */
template <class TInputImage, class TOutputImage, class TInputMesh, 
  class TOutputMesh>
class ITK_EXPORT HybridFilter:
  public ImageToImageFilter<TInputImage,TOutputImage>
{

public:

  /**
   * Standard "Self" typedef.
   */
  typedef HybridFilter  Self;

  /**
   * Standard "Superclass" typedef.
   */
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;

  /** 
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>                Pointer;
  typedef SmartPointer<const Self>      ConstPointer;


  /** 
   *  Smoothing filter type
   */
  typedef BalloonForceFilter<TInputMesh,TOutputMesh>  BalloonForceFilterType;


  /** 
   *  Derivative along one dimension filter type
   */
  typedef GibbsPriorFilter<TInputImage,TOutputImage>  GibbsPriorFilterType;

  /** 
   *  Pointer to a balloon force filter 
   */
  typedef typename BalloonForceFilterType::Pointer  BalloonForceFilterPointer;


  /** 
   *  Pointer to a gibbs prior filter 
   */
  typedef typename GibbsPriorFilterType::Pointer  GibbsPriorFilterPointer;                                  
                                  
  typedef
    ImageRegionIteratorWithIndex< TOutputImage > OutputImageIterator;
  
  /** 
   * Image Dimension
   */
  enum { ImageDimension = TInputImage::ImageDimension };

  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);

  /**
   * Set potential of the balloon force filter 
   * using the output of gibbs prior filter
   */
  void SetPotential( void );

  /**
   * Sent object region labelled by the deformable 
   * model to the gibbs prior model for parameter update 
   */
  void SetObjectRegion( void );

  /**
   * Set the balloon force filter and gibbs prior filter
   */
  void SetBalloonForceFilter(BalloonForceFilterPointer  bffilter);
  void SetGibbsPriorFilter(GibbsPriorFilterPointer  gpfilter);

  void Advance();
  void SetGibbsInput();

protected:
  HybridFilter();
  virtual ~HybridFilter() {};
  HybridFilter(const Self&) {}
  void operator=(const Self&) {}
  
  /**
   * Generate Data
   */
  virtual void GenerateData();

private:
  BalloonForceFilterPointer   m_BalloonForceFilter;
  GibbsPriorFilterPointer   m_GibbsPriorFilter;
  int m_IterNum;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHybridFilter.txx"
#endif

#endif
