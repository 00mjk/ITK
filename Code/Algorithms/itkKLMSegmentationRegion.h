/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkKLMSegmentationRegion.h
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
#ifndef _itkKLMSegmentationRegion_h
#define _itkKLMSegmentationRegion_h

#include "itkObject.h"
#include "itkSegmentationRegion.h"

#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"

namespace itk
{

/** \class KLMSegmentationRegion
 * \brief Base class for KLMSegmentationRegion object
 * 
 * itkKLMSegmentationRegion is the base class for the KLMSegmentationRegion
 * objects. It provides the basic function definitons that are inherent to a
 * KLMSegmentationRegion objects.  It is templated over the type of input and
 * output image.

 * This object supports data handling of multiband images. The object
 * accepts images in vector format, where each pixel is a vector and each 
 * element of the vector corresponds to an entry from 1 particular band of
 * a multiband dataset. 

 * We expect the user to provide the input to the routine in vector format. 
 * A single band image is treated as a vector image with a single element 
 * for every vector.
 * 
 * Data structures for a regions
 * =============================
 * A region is defined as a closed area in the image that is surrounded
 * by a list of borders objects (see itkKLMSegmentationBorder class).  
 *
 * Shown below is an initial 4x3 grid size partition of a 8x9
 * image. The initial region blocks are labeled in hexadecimal, and
 * the region borders are shown as E = border pixel, and C =
 * non-border pixel. Note that the border pixel grid is one pixel
 * larger than the region grid. The border pixel grid can be
 * considered to lie interspersed within the region pixel grid with
 * four border pixels surrounding each interior region pixel.
 *
 * \begin{center}
 * Initial regions of a 8 by 9 image with a 4 by 3 grid partition. \\
 * \begin{tabular}{|c|c|c|c|c|c|c|c|c|}
 * \hline
 *   1 & 1 & 1 & 2 & 2 & 2 & 3 & 3 & 3 \\ \hline
 *   1 & 1 & 1 & 2 & 2 & 2 & 3 & 3 & 3 \\ \hline
 *   4 & 4 & 4 & 5 & 5 & 5 & 6 & 6 & 6 \\ \hline
 *   4 & 4 & 4 & 5 & 5 & 5 & 6 & 6 & 6 \\ \hline
 *   7 & 7 & 7 & 8 & 8 & 8 & 9 & 9 & 9 \\ \hline
 *   7 & 7 & 7 & 8 & 8 & 8 & 9 & 9 & 9 \\ \hline
 *   a & a & a & b & b & b & c & c & c \\ \hline
 *   a & a & a & b & b & b & c & c & c \\ \hline
 *  \end{tabular}
 *  \end{center}
 *
 * \begin{center}
 * Region borders are shown as ``E''. \\
 * \begin{tabular}{|c|c|c|c|c|c|c|c|c|c|}
 * \hline
 *   C & C & C & E & C & C & E & C & C & C \\ \hline
 *   C & C & C & E & C & C & E & C & C & C \\ \hline
 *   E & E & E & E & E & E & E & E & E & E \\ \hline
 *   C & C & C & E & C & C & E & C & C & C \\ \hline
 *   E & E & E & E & E & E & E & E & E & E \\ \hline
 *   C & C & C & E & C & C & E & C & C & C \\ \hline
 *   E & E & E & E & E & E & E & E & E & E \\ \hline
 *   C & C & C & E & C & C & E & C & C & C \\ \hline
 *   C & C & C & E & C & C & E & C & C & C \\ \hline
 *  \end{tabular}
 *  \end{center}
 *
 * \ingroup RegionGrowingSegmentation 
 * 
 */
template <class TInputImage, class TOutputImage>
class KLMSegmentationBorder;

template <class TInputImage, class TOutputImage>
class ITK_EXPORT KLMSegmentationRegion : public SegmentationRegion<TInputImage,TOutputImage>
{
 private:
  /**
   * Type definition for an double vector.
   */
  typedef vnl_matrix<double> VecDblType;

public:
  /**
   * Standard "Self" typedef.
   */
  typedef KLMSegmentationRegion   Self;

  /**
   * Standard "Superclass" typedef
   */
  typedef SegmentationRegion<TInputImage,TOutputImage> Superclass;

  /** 
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro(KLMSegmentationRegion,SegmentationRegion);

  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);

  /**
   * Type definition for the input image.
   */
  typedef typename TInputImage::Pointer   InputImageType;

  /**
   * Type definition for the input image pixel type.
   */
  typedef typename TInputImage::PixelType InputImagePixelType;

  /**
   * Type definition for the input image pixel vector type.
   */
  typedef typename TInputImage::PixelType::VectorType InputImageVectorType;

  /**
   * Type definition for the image iterators to be used.
   */
  typedef
    ImageRegionIterator< TInputImage > InputImageIterator;

  /**
   * Type definition for vector container that stores the borders
   * associated with a current region.
   */             
  typedef 
    std::vector< KLMSegmentationBorder<TInputImage,TOutputImage>* > 
      RegionBorderVecType;

  /**
   * Type definition for the region border vector iterators to be used.
   */
  typedef typename RegionBorderVecType::iterator RegionBorderVecIterator;

  /**
   * Set the region with parameter values
   * defining the region.
   */
  void SetRegion(VecDblType regionMeanIntensity, 
                 unsigned int regionArea,
                 unsigned int label);
 

  /**
   * Set the border associated with a region.
   */
  void SetRegionBorder(KLMSegmentationBorder<TInputImage,TOutputImage> *pNewRegionBorder);

    /**
   * Set the border associated with a region.
   */
  void SetRegionBorder3d(KLMSegmentationBorder<TInputImage,TOutputImage> *pNewRegionBorder);

  /**
   * get the first border associated with a region.
   */
  KLMSegmentationBorder<TInputImage,TOutputImage> *GetFirstRegionBorder();

  /**
   * Delete a region border
   */
  void DeleteRegionBorder(KLMSegmentationBorder<TInputImage,
                                      TOutputImage> *pBorderCandidate);
  /**
   * Insert a region border
   */
  void InsertRegionBorder(RegionBorderVecIterator it,
                          KLMSegmentationBorder<TInputImage,
                                      TOutputImage> *pBorderCandidate);

  /**
   * Reorder the region borders given a candidate border after region 
   * merging
   */
  void ReorderRegionBorders(KLMSegmentationBorder<TInputImage,
                                        TOutputImage> *pBorderCandidate);

  /**
   * Get a head pointer to the vector containter storing the borders associated
   * with a region
   */
  RegionBorderVecIterator GetRegionBorderItBegin();

  /**
   * Get a tail pointer to the vector containter storing the borders associated
   * with a region
   */
  RegionBorderVecIterator GetRegionBorderItEnd();

  /**
   * Recalculate the lambda values for all the borders defining the region
   * and resort the entire border list in decending order of the lambda values
   */
  void UpdateRegionBorderLambda();

  /**
   * Function that allows printing of the region parameters using std::cout
   */
  void PrintRegionInfo();

protected:
  KLMSegmentationRegion();
  ~KLMSegmentationRegion();
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  KLMSegmentationRegion(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  RegionBorderVecType    m_RegionBorderVec;

}; // class SegmentationRegion


} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkKLMSegmentationRegion.txx"
#endif



#endif

