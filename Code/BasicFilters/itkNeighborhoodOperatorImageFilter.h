/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkNeighborhoodOperatorImageFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNeighborhoodOperatorImageFilter_h
#define __itkNeighborhoodOperatorImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkNeighborhoodOperator.h"
#include "itkImage.h"
#include "itkImageBoundaryCondition.h"

namespace itk
{
/** \class NeighborhoodOperatorImageFilter
 * \brief Applies a single NeighborhoodOperator to an image region. 
 *
 * This filter calculates successive inner products between a single
 * NeighborhoodOperator and a NeighborhoodIterator, which is swept
 * across every pixel in an image region.  For operators that are
 * symmetric across their axes, the result is a fast convolution
 * with the image region.  Apply the mirror()'d operator for
 * non-symmetric NeighborhoodOperators.
 *
 * \ingroup ImageFilters
 *
 * \sa Image
 * \sa Neighborhood
 * \sa NeighborhoodOperator
 * \sa NeighborhoodIterator
 */
template <class TInputImage, class TOutputImage>
class ITK_EXPORT NeighborhoodOperatorImageFilter :
    public ImageToImageFilter< TInputImage, TOutputImage > 
{
public:
  /** Standard "Self" & Superclass typedef. */
  typedef NeighborhoodOperatorImageFilter Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef       SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(NeighborhoodOperatorImageFilter, ImageToImageFilter);
  
  /** Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same. */
  typedef typename TOutputImage::PixelType         OutputPixelType;
  typedef typename TOutputImage::InternalPixelType OutputInternalPixelType;
  typedef typename  TInputImage::PixelType         InputPixelType;
  typedef typename  TInputImage::InternalPixelType InputInternalPixelType;

  /** Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension);
  
  /** Image typedef support. */
  typedef TInputImage  InputImageType;
  typedef TOutputImage OutputImageType;
  
  /** Typedef for generic boundary condition pointer. */
  typedef ImageBoundaryCondition<OutputImageType> *
   ImageBoundaryConditionPointerType;

  /** Superclass typedefs. */
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;

  /** Neighborhood types */
  typedef Neighborhood<OutputPixelType, itkGetStaticConstMacro(ImageDimension)> OutputNeighborhoodType;
  typedef Neighborhood<InputPixelType, itkGetStaticConstMacro(ImageDimension)> InputNeighborhoodType;
  
  /** Sets the operator that is used to filter the image. Note
   * that the operator is stored as an internal COPY (it
   * is not part of the pipeline). */
  void SetOperator(const OutputNeighborhoodType &p)
    {
    m_Operator = p;
    this->Modified();
    }

  /** Allows a user to override the internal boundary condition. Care should be
   * be taken to ensure that the overriding boundary condition is a persistent
   * object during the time it is referenced.  The overriding condition
   * can be of a different type than the default type as long as it is
   * a subclass of ImageBoundaryCondition. */
  void OverrideBoundaryCondition(const ImageBoundaryConditionPointerType i)
    { m_BoundsCondition = i; }

  /** NeighborhoodOperatorImageFilter needs a larger input requested
   * region than the output requested region.  As such,
   * NeighborhoodOperatorImageFilter needs to provide an implementation for
   * GenerateInputRequestedRegion() in order to inform the pipeline
   * execution model.
   *
   * \sa ProcessObject::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion() throw (InvalidRequestedRegionError);

protected:
  NeighborhoodOperatorImageFilter() {}
  virtual ~NeighborhoodOperatorImageFilter() {}
    
  /** NeighborhoodOperatorImageFilter can be implemented as a
   * multithreaded filter.  Therefore, this implementation provides a
   * ThreadedGenerateData() routine which is called for each
   * processing thread. The output image data is allocated
   * automatically by the superclass prior to calling
   * ThreadedGenerateData().  ThreadedGenerateData can only write to
   * the portion of the output image specified by the parameter
   * "outputRegionForThread"
   *
   * \sa ImageToImageFilter::ThreadedGenerateData(),
   *     ImageToImageFilter::GenerateData() */
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            int threadId );

  
  void PrintSelf(std::ostream& os, Indent indent) const
    {  Superclass::PrintSelf(os, indent); }
private:
  NeighborhoodOperatorImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the internal operator used to filter the image. */
  InputNeighborhoodType m_Operator;

  /** Pointer to a persistent boundary condition object used
   * for the image iterator. */
  ImageBoundaryConditionPointerType m_BoundsCondition;

};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNeighborhoodOperatorImageFilter.txx"
#endif

#endif
