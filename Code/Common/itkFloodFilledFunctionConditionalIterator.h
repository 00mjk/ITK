/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFloodFilledFunctionConditionalIterator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFloodFilledFunctionConditionalIterator_h
#define __itkFloodFilledFunctionConditionalIterator_h

#include <stack>

#include "itkIndex.h"
#include "itkSize.h"
#include "itkConditionalIterator.h"
#include "itkImage.h"

namespace itk
{

/**
 * \class FloodFilledFunctionConditionalIterator
 * \brief Iterates over a flood-filled spatial function. 
 *
 * \ingroup ImageIterators
 *
 */
template<class TImage, class TFunction>
class FloodFilledFunctionConditionalIterator: public ConditionalIterator<TImage>
{
public:
  /** Standard class typedefs. */
  typedef FloodFilledFunctionConditionalIterator Self;

  /** Type of function */
  typedef TFunction FunctionType;

  /** Type of vector used to store location info in the spatial function */
  typedef typename TFunction::InputType FunctionInputType;

  /** Index typedef support. */
  typedef typename TImage::IndexType  IndexType;

  /** Size typedef support. */
  typedef typename TImage::SizeType    SizeType;

  /** Region typedef support */
  typedef typename TImage::RegionType    RegionType;

  /** Image typedef support. */
  typedef TImage   ImageType;

  /** Internal Pixel Type */
  typedef typename TImage::InternalPixelType   InternalPixelType;

  /** External Pixel Type */
  typedef typename TImage::PixelType   PixelType;

  /** Dimension of the image the iterator walks.  This enum is needed so that
   * functions that are templated over image iterator type (as opposed to
   * being templated over pixel type and dimension) can have compile time
   * access to the dimension of the image that the iterator walks. */
  enum { NDimensions = TImage::ImageDimension };

  /** Constructor establishes an iterator to walk a particular image and a
   * particular region of that image. This version of the constructor uses
   * an explicit seed pixel for the flood fill, the "startIndex" */
  FloodFilledFunctionConditionalIterator(ImageType *imagePtr,
                                         FunctionType *fnPtr,
                                         IndexType startIndex);

  /** Constructor establishes an iterator to walk a particular image and a
   * particular region of that image. This version of the constructor
   * should be used when the seed pixel is unknown. */
  FloodFilledFunctionConditionalIterator(ImageType *imagePtr,
                                         FunctionType *fnPtr);

  /** Automatically find a seed pixel and set m_StartIndex. Does nothing
   * if a seed pixel isn't found. A seed pixel is determined by
   * traversing the input image's image's LargestPossibleRegion and
   * applying the IsPixelIncluded() test.*/
  void FindSeedPixel();

    /** Initializes the iterator, called from constructor */
  void InitializeIterator();

  /** Default Destructor. */
  virtual ~FloodFilledFunctionConditionalIterator() {};

  /** Compute whether the index of interest should be included in the flood */
  virtual bool IsPixelIncluded(const IndexType & index) const = 0;
  
  /** operator= is provided to make sure the handle to the image is properly
   * reference counted. */
  Self &operator=(const Self& it)
  {
    m_Image = it.m_Image;     // copy the smart pointer
    m_Region = it.m_Region;
  }
  
  /** Get the dimension (size) of the index. */
  static unsigned int GetIteratorDimension() 
    {return TImage::ImageDimension;}

  /** Get the index. This provides a read only reference to the index.
   * This causes the index to be calculated from pointer arithmetic and is
   * therefore an expensive operation.
   * \sa SetIndex */
  const IndexType GetIndex()
    { return m_IndexStack.top();}

  /** Get the pixel value */
  PixelType & Get(void)
    { return m_Image->GetPixel(m_IndexStack.top() ); }
  
  /** Set the pixel value */
  void Set( const PixelType & value)
    { m_Image->GetPixel(m_IndexStack.top() ) = value; }

  /** Is the iterator at the end of the region? */
  bool IsAtEnd()
    { return m_IsAtEnd; };

 /** Move an iterator to the beginning of the region. "Begin" is
  * defined as the first pixel in the region. */
  void GoToBegin()
    {
    // Clear the stack
    while (!m_IndexStack.empty())
      {
      m_IndexStack.pop();
      }

    if( this->IsPixelIncluded(m_StartIndex) )
      {
      // Push the seed onto the stack
      m_IndexStack.push(m_StartIndex);
      
      // Obviously, we're at the beginning
      m_IsAtEnd = false;

      // Initialize the temporary image
      tempPtr->FillBuffer(NumericTraits<ITK_TYPENAME TTempImage::PixelType>::Zero);

      // Mark the start index in the temp image as inside the function, neighbor check incomplete
      tempPtr->SetPixel(m_StartIndex, 2);
      }
    else
      {
      // If the start index is not included, we're done
      m_IsAtEnd = true;
      }    
    };

  /** Walk forward one index */
  void operator++()
    { this->DoFloodStep(); }

  void DoFloodStep();
  
protected: //made protected so other iterators can access 
  /** Smart pointer to the function we're evaluating */
  SmartPointer<FunctionType> m_Function;

  /** A temporary image used for storing info about indices
   * 0 = pixel has not yet been processed
   * 1 = pixel is not inside the function
   * 2 = pixel is inside the function, neighbor check incomplete
   * 3 = pixel is inside the function, neighbor check complete */
  typedef Image<unsigned char, NDimensions> TTempImage;
  typename TTempImage::Pointer tempPtr;
  
  /** A known seed location to start the recursive fill */
  IndexType m_StartIndex;

  /** The origin of the source image */
  const double* m_ImageOrigin;
  
  /** The spacing of the source image */
  const double* m_ImageSpacing;

  /** Size of the source image */
  const unsigned long int* m_ImageSize;

  /** Stack used to hold the path of the iterator through the image */
  std::stack<IndexType> m_IndexStack;

  /** Location vector used in the flood algorithm */
  FunctionInputType m_LocationVector;

  /** Indicates whether or not we've found a neighbor that needs to be checked. */
  bool m_FoundUncheckedNeighbor;

  /** Indicates whether or not an index is valid (inside an image)/ */
  bool m_IsValidIndex;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFloodFilledFunctionConditionalIterator.txx"
#endif

#endif 
