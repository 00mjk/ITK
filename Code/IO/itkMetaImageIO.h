/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkMetaImageIO.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMetaImageIO_h
#define __itkMetaImageIO_h

#include <fstream>
#include "itkImageIOBase.h"
#include "metaObject.h"
#include "metaImage.h"

namespace itk
{

/** \brief Read MetaImage file format. */
class ITK_EXPORT MetaImageIO : public ImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef MetaImageIO            Self;
  typedef ImageIOBase  Superclass;
  typedef SmartPointer<Self>  Pointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MetaImageIO, Superclass);

  /*-------- This part of the interfaces deals with reading data. ----- */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  virtual bool CanReadFile(const char*) ;

  /** Set the spacing and dimension information for the set filename. */
  virtual void ReadImageInformation();
  
  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void* buffer);

  MetaImage * GetMetaImagePointer(void);
  
  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine the file type. Returns true if this ImageIO can write the
   * file specified. */
  virtual bool CanWriteFile(const char*);

  /** Set the spacing and dimension information for the set filename. */
  virtual void WriteImageInformation();
  
  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegions has been set properly. */
  virtual void Write(const void* buffer);


protected:
  MetaImageIO();
  ~MetaImageIO();
  void PrintSelf(std::ostream& os, Indent indent) const;
  
private:
  
  MetaImage m_MetaImage;

  MetaImageIO(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  std::ifstream   m_Ifstream;

  std::ofstream   m_Ofstream;
  
};

} // end namespace itk

#endif // __itkMetaImageIO_h
