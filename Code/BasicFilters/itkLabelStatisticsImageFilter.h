/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLabelStatisticsImageFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLabelStatisticsImageFilter_h
#define __itkLabelStatisticsImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkNumericTraits.h"
#include "itkArray.h"
#include "itkSimpleDataObjectDecorator.h"
#include "itk_hash_map.h"
#include <vector>

namespace itk {

/** \class LabelStatisticsImageFilter 
 * \brief Given an intensity image and a label map, compute min, max, variance and mean of the pixels associated with each label or segment
 *
 * LabelStatisticsImageFilter computes the minimum, maximum, sum,
 * mean, variance and sigma of regions of an intensity image, where
 * the regions are defined via a label map (a second input).  The
 * label image should be integral type. The filter needs all of its
 * input image.  It behaves as a filter with an input and output. Thus
 * it can be inserted in a pipline with other filters and the
 * statistics will only be recomputed if a downstream filter changes.
 *
 * The filter passes its intensity input through unmodified.  The filter is
 * threaded. It computes statistics in each thread then combines them in
 * its AfterThreadedGenerate method.
 *
 * \ingroup MathematicalStatisticsImageFilters
 */
template<class TInputImage, class TLabelImage>
class ITK_EXPORT LabelStatisticsImageFilter : 
    public ImageToImageFilter<TInputImage, TInputImage>
{
public:
  /** Standard Self typedef */
  typedef LabelStatisticsImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TInputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(LabelStatisticsImageFilter, ImageToImageFilter);
  
  /** Image related typedefs. */
  typedef typename TInputImage::Pointer InputImagePointer;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;

  /** Label image related typedefs. */
  typedef TLabelImage LabelImageType;
  typedef typename TLabelImage::Pointer LabelImagePointer;
  typedef typename TLabelImage::RegionType LabelRegionType ;
  typedef typename TLabelImage::SizeType LabelSizeType ;
  typedef typename TLabelImage::IndexType LabelIndexType ;
  typedef typename TLabelImage::PixelType LabelPixelType ;
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension ) ;

  /** Type to use form computations. */
  typedef typename NumericTraits<PixelType>::RealType RealType;

  /** Smart Pointer type to a DataObject. */
  typedef typename DataObject::Pointer DataObjectPointer;

  /** Type of DataObjects used for scalar outputs */
  typedef SimpleDataObjectDecorator<RealType> RealObjectType;

  /** Statistics stored per label */
  class LabelStatistics
  {
  public:
    LabelStatistics()
      {
        // initialized to the default values
        m_Count = 0;
        m_Sum = NumericTraits<RealType>::Zero;
        m_SumOfSquares = NumericTraits<RealType>::Zero;

        // Set such that the first pixel encountered can be compared
        m_Minimum = NumericTraits<RealType>::max();
        m_Maximum = NumericTraits<RealType>::NonpositiveMin();

        // Default these to zero
        m_Mean = NumericTraits<RealType>::Zero;
        m_Sigma = NumericTraits<RealType>::Zero;
        m_Variance = NumericTraits<RealType>::Zero;
      }
    
    unsigned long m_Count;
    RealType m_Minimum;
    RealType m_Maximum;
    RealType m_Mean;
    RealType m_Sum;
    RealType m_SumOfSquares;
    RealType m_Sigma;
    RealType m_Variance;
  };
  
  /** Type of the map used to store data per label */
  typedef itk::hash_map<LabelPixelType, LabelStatistics> MapType;
  typedef typename itk::hash_map<LabelPixelType, LabelStatistics>::iterator MapIterator;
  typedef typename itk::hash_map<LabelPixelType, LabelStatistics>::const_iterator MapConstIterator;

  /** Set the label image */
  void SetLabelInput(TLabelImage *input)
    {
      // Process object is not const-correct so the const casting is required.
      this->SetNthInput(1, const_cast<TLabelImage *>(input) );
    }

  /** Get the label image */
  LabelImageType * GetLabelInput()
    {
      return static_cast<LabelImageType*>(const_cast<DataObject *>(this->ProcessObject::GetInput(1)));
    }

  /** Does the specified label exist? Can only be called after a call
   * a call to Update(). */
  bool HasLabel(LabelPixelType label) const
    {
      return m_LabelStatistics.find(label) != m_LabelStatistics.end();
    }

  /** Get the number of labels used */
  unsigned long GetNumberOfObjects() const
    {
      return m_LabelStatistics.size();
    }
  unsigned long GetNumberOfLabels() const
    {
      return this->GetNumberOfObjects();
    }

  
  /** Return the computed Minimum for a label. */
  RealType GetMinimum(LabelPixelType label) const;

  /** Return the computed Maximum for a label. */
  RealType GetMaximum(LabelPixelType label) const;

  /** Return the computed Mean for a label. */
  RealType GetMean(LabelPixelType label) const;

  /** Return the computed Standard Deviation for a label. */
  RealType GetSigma(LabelPixelType label) const;

  /** Return the computed Variance for a label. */
  RealType GetVariance(LabelPixelType label) const;

  /** Return the compute Sum for a label. */
  RealType GetSum(LabelPixelType label) const;

  /** Return the number of pixels for a label. */
  unsigned long GetCount(LabelPixelType label) const;

protected:
  LabelStatisticsImageFilter();
  ~LabelStatisticsImageFilter(){};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Pass the input through unmodified. Do this by Grafting in the AllocateOutputs method. */
  void AllocateOutputs();      

  /** Initialize some accumulators before the threads run. */
  void BeforeThreadedGenerateData ();
  
  /** Do final mean and variance computation from data accumulated in threads. */
  void AfterThreadedGenerateData ();
  
  /** Multi-thread version GenerateData. */
  void  ThreadedGenerateData (const RegionType& 
                              outputRegionForThread,
                              int threadId) ;

  // Override since the filter needs all the data for the algorithm
  void GenerateInputRequestedRegion();

  // Override since the filter produces all of its output
  void EnlargeOutputRequestedRegion(DataObject *data);

private:
  LabelStatisticsImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<MapType>  m_LabelStatisticsPerThread;
  MapType               m_LabelStatistics;

} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLabelStatisticsImageFilter.txx"
#endif

#endif
