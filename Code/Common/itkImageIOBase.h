/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageIOBase.h
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
// ***** NOTE REMOVE ME TO COMPILE
#define __itkImageIOBase_h
#ifndef __itkImageIOBase_h
#define __itkImageIOBase_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkIndent.h"
#include <string>

namespace itk
{

class ITK_EXPORT ImageIOBase : public Object
{
public:
  /** Standard class typedefs. */
  typedef ImageIOBase            Self;
  typedef SmartPointer<Self>  Pointer;
  typedef Object  Superclass;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageIOBase, Superclass);

  /** Run-time type information (and related methods). */
  typedef enum
  {
    ITK_UCHAR,              // aka uint8_t
    ITK_CHAR,
    ITK_USHORT,             // aka uint16_t
    ITK_SHORT,
    ITK_UINT,               // aka uint32_t
    ITK_INT,
    ITK_ULONG,              // aka uint64_t
    ITK_LONG,
    ITK_FLOAT,
    ITK_DOUBLE
  } PixelType;        // enumerated constants for the different data types

  /** Set the filename. */
  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);
  
  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  virtual bool CanReadFile(const char*) const = 0;

  /** Set/Get the type of the pixel. Often this is set during the read
   * operation and does not always need to be set. */
  itkGetMacro(PixelType, typeid);

  /** Specify the region of the data to load. */
  void SetLoadRegion(ImageIORegion region);

  /** Loads the data from disk into internal memory (the RequestedRegionData). */
  virtual void Load() = 0;
  
  /** Loads the data from disk into the memory buffer provided. */
  virtual void Load(void* buffer) = 0;

  /** The guts of this class. Returns the data in the requested region, 
   * which holds the raw pixels of the image read from disk. */
  void* GetRequestedRegionData() const;

  /** Get the image origin. */
  virtual const double* GetOrigin() const =0;

  /** Get the image spacing. */
  virtual const double* GetSpacing() const =0;

  /** Get the number of components per pixel in the image. This may
   * be set by the reading process. */
  itkGetConstMacro(NumberOfComponents,unsigned int);
  
  /** Get the number of independent variables (dimensions) in the image
   * being read. */
  itkGetMacro(NumberOfDimensions, unsigned int);

  /** Convenient method for accessing the number of bytes to get to 
   * the next pixel. Returns m_Strides[1]; */
  virtual unsigned int GetPixelStride () const;

protected:
  ImageIOBase();
  ~ImageIOBase();
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Set the number of components per pixel in the image. This may
   * be set by the reading process. */
  itkSetMacro(NumberOfComponents,unsigned int);
  
  /** Set the number of independent variables (dimensions) in the image
   * being read. */
  itkSetMacro(NumberOfDimensions, unsigned int);

  /** Return and set the size in x, y, z, etc. dimensions. */
  virtual unsigned int GetDimensions(unsigned int i) const;

  /** Set/Get the type of the pixel. Often this is set during the read
   * operation and does not always need to be set. */
  itkSetMacro(PixelType, PixelType);

  /** Does the ImageIOBase object have enough info to be of use? */
  bool m_Initialized;

  /** Filename: pathname + filename + file extension. */
  std::string m_FileName;

  /** Type of the pixel. */
  PixelType  m_PixelType;

  /** The region to load. */
  ImageIORegion m_Region;

  /** Stores the number of components per pixel. This will be 1 for 
   * grayscale images, 3 for RGBPixel images, and 4 for RGBPixelA images. */
  unsigned int m_NumberOfComponents;

  /** The number of dimensions in the image. */
  unsigned int m_NumberOfDimensions;

  /** The array which stores the number of pixels in the x, y, z directions. */
  unsigned int m_Dimensions[ITK_MAX_DIMENSIONS];

  /** Stores the number of bytes it takes to get to the next 'thing'
   * e.g. component, pixel, row, slice, etc. */
  unsigned int m_Strides[ITK_MAX_DIMENSIONS];

  /** Stores the raw pixels of the image */
  void* m_RequestedRegionData;

  /** Return the object to an initialized state, ready to be used */
  virtual void Reset(const bool freeDynamic = true);

  /** Resize the ImageIOBase object to new dimensions. */
  void Resize(const unsigned int numDimensions, 
              const unsigned int* dimensions);

  /** Calculates the different strides (distance from one thing to the next).
   * Upon return,
   * strides[0] = bytes to get to the next component of a pixel,
   * strides[1] = bytes to get to the next pixel in x direction,
   * strides[2] = bytes to get to the next row in y direction,
   * strides[3] = bytes to get to the next slice in z direction, etc. */
  void ComputeStrides();

  /** Return the number of pixels in the image. */
  unsigned int ImageSizeInPixels() const;

  /** Return the number of pixels times the number 
   * of components in the image. */
  unsigned int ImageSizeInComponents() const;

  /** Return the number of bytes in the image. */
  unsigned int ImageSizeInBytes() const;

  /** Convenient method for accessing number of bytes to get to the next pixel 
   * component. Returns m_Strides[0]. */
  unsigned int GetComponentStride() const;

  /** Convenient method for accessing the number of bytes to get to the 
   * next row. Returns m_Strides[2]. */
  unsigned int GetRowStride () const;

  /** Convenient method for accessing the number of bytes to get to the 
   * next slice. Returns m_Strides[3]. */
  unsigned int GetSliceStride () const;

private:
  ImageIOBase(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  

};

} // end namespace itk

#endif // __itkImageIOBase_h
