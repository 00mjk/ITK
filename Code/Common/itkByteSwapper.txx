/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkByteSwapper.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkByteSwapper_txx
#define _itkByteSwapper_txx
#include "itkByteSwapper.h"
#include "itkExceptionObject.h"
#include "itkObjectFactory.h"
#include <memory>

namespace itk
{

/** \class ByteSwapperError
 * Exception thrown when trying to swap type of unexpected
 * number of bytes.
 */
class ByteSwapperError : public ExceptionObject
{
public:
  /** Default constructor.  Needed to ensure the exception object can be copied. */
  ByteSwapperError() : ExceptionObject() {}
  
  /** Constructor. Needed to ensure the exception object can be copied. */
  ByteSwapperError(const char *file, unsigned int lineNumber) : ExceptionObject(file, lineNumber) {}

  /** Constructor. Needed to ensure the exception object can be copied. */
  ByteSwapperError(const std::string& file, unsigned int lineNumber) : ExceptionObject(file, lineNumber) {}  
  
  itkTypeMacro(ByteSwapperError, ExceptionObject);
};


// The following are the public methods --------------------------------
//

  // Machine definitions
#ifdef CMAKE_WORDS_BIGENDIAN
  template <class T>
  bool ByteSwapper<T>::IsBigEndian() { return true; }
  template <class T>
  bool ByteSwapper<T>::IsLittleEndian() { return false; }
#else
  template <class T>
  bool ByteSwapper<T>::IsBigEndian() { return false; }
  template <class T>
  bool ByteSwapper<T>::IsLittleEndian() { return true; }
#endif  

  
//------Big Endian methods----------------------------------------------

// Use different swap methods based on type
#ifdef CMAKE_WORDS_BIGENDIAN
template <class T>
void 
ByteSwapper<T>
::SwapBE(T *){}
#else
template <class T>
void 
ByteSwapper<T>
::SwapBE(T *p)
{
  switch ( sizeof(T) )
    {
    case 1:
      return;
    case 2:
      ByteSwapper<T>::Swap2((void *)p);
      return;
    case 4:
      ByteSwapper<T>::Swap4((void *)p);      
      return;
    default:  
      ByteSwapperError e(__FILE__, __LINE__);
      e.SetLocation("SwapBE");
      e.SetDescription("Cannot swap number of bytes requested");
      throw e;
    }
}
#endif  

#ifdef CMAKE_WORDS_BIGENDIAN
template <class T>
void 
ByteSwapper<T>
::SwapRangeBE(T *p, unsigned long num) {}
#else
template <class T>
void 
ByteSwapper<T>
::SwapRangeBE(T *p, unsigned long num)
{
  switch ( sizeof(T) )
    {
    case 1:
      return;
    case 2:
      ByteSwapper<T>::Swap2Range((void *)p,num);
      return;
    case 4:
      ByteSwapper<T>::Swap4Range((void *)p,num);      
      return;
    default:  
      ByteSwapperError e(__FILE__, __LINE__);
      e.SetLocation("SwapRangeBE");
      e.SetDescription("Cannot swap number of bytes requested");
      throw e;
    }
}
#endif  

#ifdef CMAKE_WORDS_BIGENDIAN
template <class T>
void 
ByteSwapper<T>
::SwapWriteRangeBE(T *p, int num, OStreamType *fp)
{
  num *= sizeof(T);
  fp->write((char *)p, num);
}
#else
template <class T>
void 
ByteSwapper<T>
::SwapWriteRangeBE(T *p, int num, OStreamType *fp)
{
  switch ( sizeof(T) )
    {
    case 1:
      return;
    case 2:
      ByteSwapper<T>::SwapWrite2Range((void *)p, num, fp);
      return;
    case 4:
      ByteSwapper<T>::SwapWrite4Range((void *)p, num, fp);      
      return;
    default:  
      ByteSwapperError e(__FILE__, __LINE__);
      e.SetLocation("SwapWriteRangeBE");
      e.SetDescription("Cannot swap number of bytes requested");
      throw e;
    }
}
#endif

//------Little Endian methods----------------------------------------------

#ifdef CMAKE_WORDS_BIGENDIAN
template <class T>
void 
ByteSwapper<T>
::SwapLE(T *p)
{
  switch ( sizeof(T) )
    {
    case 1:
      return;
    case 2:
      ByteSwapper<T>::Swap2((void *)p);
      return;
    case 4:
      ByteSwapper<T>::Swap4((void *)p);      
      return;
    default:  
      ByteSwapperError e(__FILE__, __LINE__);
      e.SetLocation("SwapLE");
      e.SetDescription("Cannot swap number of bytes requested");
      throw e;
    }
}
#else
template <class T>
void 
ByteSwapper<T>
::SwapLE(T *){}
#endif  


#ifdef CMAKE_WORDS_BIGENDIAN
template <class T>
void 
ByteSwapper<T>
::SwapRangeLE(T *p, unsigned long num) 
{
  switch ( sizeof(T) )
    {
    case 1:
      return;
    case 2:
      ByteSwapper<T>::Swap2Range((void *)p,num);
      return;
    case 4:
      ByteSwapper<T>::Swap4Range((void *)p,num);      
      return;
    default:  
      ByteSwapperError e(__FILE__, __LINE__);
      e.SetLocation("SwapRangeLE");
      e.SetDescription("Cannot swap number of bytes requested");
      throw e;
    }
}
#else
template <class T>
void 
ByteSwapper<T>
::SwapRangeLE(T *p, unsigned long num) {}
#endif  

#ifdef CMAKE_WORDS_BIGENDIAN
template <class T>
void 
ByteSwapper<T>
::SwapWriteRangeLE(T *p, int num, OStreamType *fp)
{
  switch ( sizeof(T) )
    {
    case 1:
      return;
    case 2:
      ByteSwapper<T>::SwapWrite2Range((void *)p, num, fp);
      return;
    case 4:
      ByteSwapper<T>::SwapWrite4Range((void *)p, num, fp);      
      return;
    default:  
      ByteSwapperError e(__FILE__, __LINE__);
      e.SetLocation("SwapWriteRangeLE");
      e.SetDescription("Cannot swap number of bytes requested");
      throw e;
    }
}
#else
template <class T>
void 
ByteSwapper<T>
::SwapWriteRangeLE(T *p, int num, OStreamType *fp)
{
  num *= sizeof(T);
  fp->write((char *)p, num);
}
#endif

// The following are the protected methods -------------------------
//

//------2-byte methods----------------------------------------------

// Swap 2 byte word.
template <class T>
void 
ByteSwapper<T>
::Swap2(void *pin)
{
  unsigned short h1,h2;
  short* p = reinterpret_cast<short*>(pin);
  
  h1 = static_cast<unsigned short>(*p) << 8;
  h2 = static_cast<unsigned short>(*p) >> 8;
  *p = (short) h1 | h2;
}

// Swap bunch of bytes. Num is the number of two byte words to swap.
template <class T>
void 
ByteSwapper<T>
::Swap2Range(void *ptr, unsigned long num)
{
  char one_byte;
  char *pos = reinterpret_cast<char *>(ptr);
  unsigned long i;
  
  for (i = 0; i < num; i++)
    {
    one_byte = pos[0];
    pos[0] = pos[1];
    pos[1] = one_byte;
    pos = pos + 2;
    }
}

// Swap bunch of bytes. Num is the number of four byte words to swap.
template <class T>
void 
ByteSwapper<T>
::SwapWrite2Range(void *ptr, unsigned long num, OStreamType *fp)
{
  char one_byte;
  char *pos;
  unsigned long i;
  char *cpy;
  unsigned long chunkSize = 1000000;

  if (num < chunkSize)
    {
    chunkSize = num;
    }
  cpy = new char [chunkSize * 2];
 
  while (num)
    {
    memcpy(cpy, ptr, chunkSize * 2);
 
    pos = cpy; 
    for (i = 0; i < chunkSize; i++)
      {
      one_byte = pos[0];
      pos[0] = pos[1];
      pos[1] = one_byte;
      pos = pos + 2;
      }
    fp->write((char *)cpy, 2*chunkSize);
    ptr = (char *) ptr + chunkSize * 2;
    num -= chunkSize;
    if (num < chunkSize)
      {
      chunkSize = num;
      }
    }
  delete [] cpy;
}

//------4-byte methods----------------------------------------------

// Swap four byte word.
template <class T>
void 
ByteSwapper<T>
::Swap4(void *ptr)
{
  char one_byte;
  char *p = reinterpret_cast<char *>(ptr);

  one_byte    = p[0];
  p[0] = p[3];
  p[3] = one_byte;

  one_byte    = p[1];
  p[1] = p[2];
  p[2] = one_byte;
}

// Swap bunch of bytes. Num is the number of four byte words to swap.
template <class T>
void 
ByteSwapper<T>
::Swap4Range(void *ptr, unsigned long num)
{
  char one_byte;
  char *pos = reinterpret_cast<char *>(ptr);
  unsigned long i;

  for (i = 0; i < num; i++)
    {
    one_byte = pos[0];
    pos[0] = pos[3];
    pos[3] = one_byte;
    
    one_byte = pos[1];
    pos[1] = pos[2];
    pos[2] = one_byte;
    pos = pos + 4;
    }
}

// Swap bunch of bytes. Num is the number of four byte words to swap.
template <class T>
void 
ByteSwapper<T>
::SwapWrite4Range(void *ptr, unsigned long num, OStreamType *fp)
{
  char one_byte;
  char *pos;
  unsigned long i;
  char *cpy;
  unsigned long chunkSize = 1000000;

  if (num < chunkSize)
    {
    chunkSize = num;
    }
  cpy = new char [chunkSize * 4];
 
  while (num)
    {
    memcpy(cpy, ptr, chunkSize * 4);
    
    pos = cpy;   
    for (i = 0; i < chunkSize; i++)
      {
      one_byte = pos[0];
      pos[0] = pos[3];
      pos[3] = one_byte;
      
      one_byte = pos[1];
      pos[1] = pos[2];
      pos[2] = one_byte;
      pos = pos + 4;
      }
    fp->write((char *)cpy, 4*chunkSize);
    ptr  = (char *) ptr + chunkSize*4;
    num -= chunkSize;
    if (num < chunkSize)
      {
      chunkSize = num;
      }
    }
  delete [] cpy;
}

} // end namespace itk

#endif
