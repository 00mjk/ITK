/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageFileReader_h
#define __itkImageFileReader_h

#include "itkImageIOBase.h"
#include "itkImageSource.h"
#include "itkExceptionObject.h"
#include "itkSize.h"
#include "itkImageRegion.h"
#include "itkDefaultConvertPixelTraits.h"

namespace itk
{

/** \brief Base exception class for IO conflicts. */
class ImageFileReaderException : public ExceptionObject 
{
public:
  /** Run-time information. */
  itkTypeMacro( ImageFileReaderException, ExceptionObject );

  /** Constructor. */
  ImageFileReaderException(char *file, unsigned int line, 
                           const char* message = "Error in IO") : 
    ExceptionObject(file, line)
    {
      SetDescription(message);
    }

  /** Constructor. */
  ImageFileReaderException(const std::string &file, unsigned int line, 
                           const char* message = "Error in IO") : 
    ExceptionObject(file, line)
    {
      SetDescription(message);
    }
};


/** \brief Data source that reads image data from disk files.
 *
 * This source object is a general filter to read data from
 * a variety of file formats. It works with a ImageIOBase subclass
 * to actually do the reading of the data. Object factory machinery
 * can be used to automatically create the ImageIOBase, or the
 * ImageIOBase can be manually created and set.
 *
 * TOutputImage is the type expected by the external users of the
 * filter. If data stored in the file is stored in a different format
 * then specified by TOutputImage, than this filter converts data 
 * between the file type and the external expected type.  The 
 * ConvertTraits template argument is used to do the conversion.
 *
 * A Pluggable factory pattern is used this allows different kinds of readers
 * to be registered (even at run time) without having to modify the
 * code in this class.
 */
template <class TOutputImage,
  class ConvertPixelTraits = 
DefaultConvertPixelTraits< ITK_TYPENAME TOutputImage::PixelType> >
class ITK_EXPORT ImageFileReader : public ImageSource<TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef ImageFileReader         Self;
  typedef ImageSource<TOutputImage>  Superclass;
  typedef SmartPointer<Self>  Pointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageFileReader, ImageSource);

  /** The size of the output image. */
  typedef Size<TOutputImage::ImageDimension>  SizeType;

  /** The region of the output image. */
  typedef ImageRegion<TOutputImage::ImageDimension>  ImageRegionType;

  /** The pixel type of the output image. */
  typedef typename TOutputImage::PixelType OutputImagePixelType;
  
  /** Specify the file to load. This is forwarded to the IO instance. 
   * Either the FileName or FilePrefix plus pattern are used to read
   * files. */
  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);
  
  /** Specify file prefix for the image file(s). You should specify either
   * a FileName or FilePrefix. Use FilePrefix if the data is stored
   * in multiple files. */
  itkSetStringMacro(FilePrefix);
  itkGetStringMacro(FilePrefix);
  
  /** The sprintf format used to build filename from FilePrefix and number. */
  itkSetStringMacro(FilePattern);
  itkGetStringMacro(FilePattern);
  
  /** Set/Get the ImageIO helper class. Often this is created via the object
   * factory mechanism that determines whether a particular ImageIO can
   * read a certain file. This method provides a way to get the ImageIO 
   * instance that is created. */
  itkSetObjectMacro(ImageIO,ImageIOBase);
  itkGetObjectMacro(ImageIO,ImageIOBase);
  
  /** Prepare the allocation of the output image during the first back
      propagation of the pipeline */
  virtual void GenerateOutputInformation(void);
  
protected:
  ImageFileReader();
  ~ImageFileReader();
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Convert a block of pixels from one type to another. */
  void DoConvertBuffer(void* buffer, unsigned long numberOfPixels);

  /** Does the real work. */
  virtual void GenerateData();

  ImageIOBase::Pointer m_ImageIO;
  bool m_UserSpecified; //keep track whether the ImageIO is user specified

  std::string m_FileName;
  std::string m_FilePrefix;
  std::string m_FilePattern;
  
private:
  ImageFileReader(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
};


} //namespace ITK

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageFileReader.txx"
#endif

#endif // __itkImageFileReader_h
