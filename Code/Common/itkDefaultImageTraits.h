/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkDefaultImageTraits.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDefaultImageTraits_h
#define __itkDefaultImageTraits_h

#include "itkImageRegion.h"
#include "itkValarrayImageContainer.h"

namespace itk
{


/**
 * Default ImageTraits for any PixelType.
 *
 * \sa Image
 * \ingroup ImageObjects
 */
template <typename TPixelType,
          unsigned int VImageDimension,
          typename TPixelContainer = ValarrayImageContainer<unsigned long, TPixelType> >
class DefaultImageTraits
{
public:
  /** The pixel type of the image. */
  typedef TPixelType PixelType;

  /** The dimension of the image. */
  enum { ImageDimension = VImageDimension };
  
  /** The container of Pixels for the image. */
  typedef TPixelContainer PixelContainer;

  /** Index typedef support. An index is used to access pixel values. */
  typedef Index<ImageDimension>  IndexType;

  /** Offset typedef support. An offset is used to access pixel values. */
  typedef Offset<ImageDimension>  OffsetType;

  /** Size typedef support. A size is used to define region bounds. */
  typedef Size<ImageDimension>  SizeType;

  /** Region typedef support. A region is used to specify a subset of an image. */
  typedef ImageRegion<ImageDimension>  RegionType;
};


} // namespace itk

#endif
