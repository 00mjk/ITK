/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkIsolatedConnectedImageFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkIsolatedConnectedImageFilter_h
#define __itkIsolatedConnectedImageFilter_h

#include "itkImage.h"
#include "itkImageToImageFilter.h"

namespace itk{

/** /class IsolatedConnectedImageFilter
 * \brief Label pixels that are connected to one sedd but not another
 * IsolatedConnectedImageFilter labels pixels with ReplaceValue that are
 * connected to Seed1 AND NOT connected to Seed2. The filter adjusts the
 * upper threshold until the two seeds are not connected. The user
 * supplies a Lower threshold. The algorithm uses a binary search to
 * adjust the upper threshold, starting at the largest possible value for
 * the InputImagePixelType.
 *
 * \ingroup RegionGrowingSegmentation 
 */
template <class TInputImage, class TOutputImage>
class IsolatedConnectedImageFilter:
  public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef IsolatedConnectedImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods).  */
  itkTypeMacro(IsolatedConnectedImageFilter,
               ImageToImageFilter);

  typedef TInputImage InputImageType;
  typedef typename InputImageType::Pointer InputImagePointer;
  typedef typename InputImageType::RegionType InputImageRegionType; 
  typedef typename InputImageType::PixelType InputImagePixelType; 
  typedef typename InputImageType::IndexType IndexType;
  typedef typename InputImageType::SizeType SizeType;
  
  typedef TOutputImage OutputImageType;
  typedef typename OutputImageType::Pointer OutputImagePointer;
  typedef typename OutputImageType::RegionType OutputImageRegionType; 
  typedef typename OutputImageType::PixelType OutputImagePixelType; 
  
  void PrintSelf ( std::ostream& os, Indent indent ) const;

  /** Set seed point 1. This seed will be isolated from Seed2 (if possible).
   *  All pixels connected to this seed will be replaced with ReplaceValue.
   */
  itkSetMacro(Seed1, IndexType);

  /** Set seed point 2. This seed will be isolated from Seed1 (if possible). */
  itkSetMacro(Seed2, IndexType);

  /** Set/Get the lower threshold. The default is 0. */
  itkSetMacro(Lower, InputImagePixelType);
  itkGetMacro(Lower, InputImagePixelType);

  /** Set/Get value to replace thresholded pixels. Pixels that lie *
   *  within Lower and Upper (inclusive) will be replaced with this
   *  value. The default is 1. */
  itkSetMacro(ReplaceValue, OutputImagePixelType);
  itkGetMacro(ReplaceValue, OutputImagePixelType);

  /** Get value that isolates the two seeds. */
  itkGetMacro(IsolatedValue, OutputImagePixelType);

protected:
  IsolatedConnectedImageFilter();
  ~IsolatedConnectedImageFilter(){};
  IndexType m_Seed1;
  IndexType m_Seed2;
  InputImagePixelType m_Lower;
  OutputImagePixelType m_ReplaceValue;
  InputImagePixelType m_IsolatedValue;
  void GenerateInputRequestedRegion();
  void GenerateData();
  
private:
  IsolatedConnectedImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkIsolatedConnectedImageFilter.txx"
#endif

#endif
