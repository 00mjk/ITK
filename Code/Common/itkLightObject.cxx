/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLightObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.

=========================================================================*/
#include "itkLightObject.h"
#include "itkObjectFactory.h"

namespace itk
{

/**
 * Instance creation.
 */
LightObject::Pointer
LightObject
::New()
{
  Self *ret = ObjectFactory<Self>::Create();
  if(ret)
    {
    return ret;
    }
  return new Self;
}
  
  
/**
 * Delete a itk object. This method should always be used to delete an object 
 * when the new operator was used to create it. Using the C++ delete method
 * will not work with reference counting.
 */
void 
LightObject
::Delete() 
{
  this->UnRegister();
}


/**
 * Avoid DLL boundary problems.
 */
#ifdef _WIN32
void* 
LightObject
::operator new(size_t nSize, const char *, int)
{
  void* p=malloc(nSize);
  return p;
}

void* 
LightObject
::operator new(size_t nSize)
{
  void* p=malloc(nSize);
  return p;
}

void 
LightObject
::operator delete( void *p )
{
  free(p);
}
#endif 


/**
 * This function will be common to all itk objects.  It just calls the
 * header/self/trailer virtual print methods, which can be overriden by
 * subclasses (any itk object).
 */
void 
LightObject
::Print(std::ostream& os)
{
  Indent indent;

  this->PrintHeader(os,0); 
  this->PrintSelf(os, indent.GetNextIndent());
  this->PrintTrailer(os,0);
}


/**
 * This method is called when itkErrorMacro executes. It allows 
 * the debugger to break on error.
 */
void 
LightObject
::BreakOnError()
{
  ;  
}


/**
 * Increase the reference count (mark as used by another object).
 */
void 
LightObject
::Register()
{
  m_ReferenceCount++;
  if(m_ReferenceCount <= 0)
    {
    delete this;
    }
}


/**
 * Decrease the reference count (release by another object).
 */
void 
LightObject
::UnRegister()
{
  if(--m_ReferenceCount <= 0)
    {
    /**
     * If there is a delete method, invoke it.
     */
    if(m_DeleteMethod)
      {
      (*m_DeleteMethod)(this);
      }
    delete this;
    }
}


/**
 * Sets the reference count (use with care)
 */
void 
LightObject
::SetReferenceCount(int ref)
{
  m_ReferenceCount = ref;
}


/**
 * Access routine to set the delete method pointer.
 */
void 
LightObject
::SetDeleteMethod(void (*f)(void *))
{
  m_DeleteMethod = f;
}


/**
 * Create an object with a reference count of 1.
 */
LightObject
::LightObject():
  /**
   * initial ref count = 0 because smart pointer immediately increments it
   */
  m_ReferenceCount(0),
  m_DeleteMethod(NULL)
{
}


LightObject
::~LightObject() 
{
  /**
   * warn user if reference counting is on and the object is being referenced
   * by another object
   */
  if ( m_ReferenceCount > 0)
    {
    itkErrorMacro(<< "Trying to delete object with non-zero reference count.");
    }
}


/**
 * Chaining method to print an object's instance variables, as well as
 * its superclasses.
 */
void 
LightObject
::PrintSelf(std::ostream& os, Indent indent)
{
  if(m_DeleteMethod)
    {
    os << indent << "Delete Method defined" << std::endl;
    }
  else
    {
    os << indent <<"No Delete Method" << std::endl;
    }
  os << indent << "Reference Count: " << m_ReferenceCount << std::endl;
}


/**
 * Define a default print header for all objects.
 */
void 
LightObject
::PrintHeader(std::ostream& os, Indent indent)
{
  os << indent << this->GetClassName() << " (" << this << ")\n";
}


/**
 * Define a default print trailer for all objects.
 */
void 
LightObject
::PrintTrailer(std::ostream& os, Indent indent)
{
  os << indent << std::endl;
}


/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream& 
operator<<(std::ostream& os, LightObject& o)
{
  o.Print(os);
  return os;
}

} // end namespace itk
