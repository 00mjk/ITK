/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLightObject.h
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
#ifndef __itkLightObject_h
#define __itkLightObject_h

#include "itkMacro.h"

#include <iostream>
#include <typeinfo>

#include "itkSmartPointer.h"
#include "itkTimeStamp.h"
#include "itkIndent.h"

namespace itk
{
class SubjectImplementation;
class Command;
class SimpleFastMutexLock;
  
/** \class LightObject
 * \brief Light weight base class for most itk classes.
 * 
 * LightObject is the highest level base class for most itk objects. It
 * implements reference counting, call-backs, and API for object printing.
 * It can be used as a lightweight base class in preference to Object.
 *
 * \sa Object
 * \ingroup ITKSystemObjects
 * \ingroup DataRepresentation
 */
class ITK_EXPORT LightObject 
{
public:
  /**
   * Standard "Self" typedef.
   */
  typedef LightObject         Self;

  /**
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /**
   * Method for creation through the object factory.
   */
  static Pointer New();

  /** 
   * Delete an itk object.  This method should always be used to delete an
   * object when the new operator was used to create it. Using the C
   *  delete method will not work with reference counting. 
   */
  virtual void Delete();

  /** 
   * Return the name of this class as a string. Used by the object factory
   * (implemented in New()) to instantiate objects of a named type. Also
   * used for debugging and other output information. 
   */
  virtual const char *GetNameOfClass() const 
    {return "LightObject";}

#ifdef _WIN32
  /** 
   * Used to avoid dll boundary problems. 
   */
  void* operator new(size_t);
  void* operator new[](size_t);
  void operator delete(void*);
  void operator delete[](void*, size_t);
#endif 
  
  /** 
   * Cause the object to print itself out.
   */
  void Print(std::ostream& os) const;

  /** 
   * This method is called when itkErrorMacro executes. It allows 
   * the debugger to break on error. 
   */
  static void BreakOnError();
  
  /** 
   * Increase the reference count (mark as used by another object). 
   */
  virtual void Register() const;

  /** 
   * Decrease the reference count (release by another object). 
   */
  virtual void UnRegister() const;

  /**
   * Gets the reference count on this object.
   */
  virtual int GetReferenceCount() const 
    {return m_ReferenceCount;}

  /** 
   * Sets the reference count on this object. This is a dangerous
   * method, use it with care.
   */
  virtual void SetReferenceCount(int);

  /**
   * Allow people to add/remove/invoke observers (callbacks) to any ITK
   * object This is an implementation of the subject/observer design
   * pattern. An observer is added by specifying an event to respond to
   * and an itk::Command to execute. It returns an unsigned long tag
   * which can be used later to remove the event or retrieve the
   * command.  The memory for the Command becomes the responsibility of
   * this object, so don't pass the same instance of a command to two
   * different objects 
   */
  unsigned long AddObserver(unsigned long event, Command *);

  /**
   * Add observer based on the string name of the event.
   */
  unsigned long AddObserver(const char *event, Command *);
  
  /** 
   * Get the command associated with the given tag.  NOTE: This returns
   * a pointer to a Command, but it is safe to asign this to a
   * Command::Pointer.  Since Command inherits from LightObject, at this
   * point in the code, only a pointer or a reference to the Command can
   * be used.  
   */
  Command* GetCommand(unsigned long tag);

  /**
   * Call Execute on all the Commands observing this event id.
   */
  void InvokeEvent(unsigned long event);

  /**
   * Call Execute on all the Commands observing this event id.
   * The actions triggered by this call doesn't modify this object.
   */
  void InvokeEvent(unsigned long event) const;
 
  /**
   * Call Execute on all the Commands observing this event, convert the
   * string to id first.  
   */
  void InvokeEvent(const char *event);

  /**
   * Call Execute on all the Commands observing this event, convert the
   * string to id first.  
   * The actions triggered by this call doesn't modify this object.
   */
  void InvokeEvent(const char *event) const;


  /**
   * Remove the observer with this tag value.
   */
  void RemoveObserver(unsigned long tag);

  /**
   * Return true if an observer is registered for this event.
   */
  bool HasObserver(unsigned long event);

  /**
   * Return true if an observer is registered for this event.
   */
  bool HasObserver(const char *event);
  
protected:
  LightObject(); 
  virtual ~LightObject(); 
  LightObject(const Self&) {}
  void operator=(const Self&) {}

  /** 
   * Methods invoked by Print() to print information about the object
   * including superclasses. Typically not called by the user (use Print()
   * instead) but used in the hierarchical print process to combine the
   * output of several classes. 
   */
  virtual void PrintSelf(std::ostream& os, Indent indent) const;
  virtual void PrintHeader(std::ostream& os, Indent indent) const;
  virtual void PrintTrailer(std::ostream& os, Indent indent) const;

  /**
   * Number of uses of this object by other objects.
   */
  mutable int m_ReferenceCount;

  /**
   * Mutex lock to protect modification to the reference count
   */
  mutable SimpleFastMutexLock *m_ReferenceCountLock;
  
  /**
   * Implementation class for Subject/Observer Pattern.
   * This is only allocated if used.
   */
  SubjectImplementation* m_SubjectImplementation;
};

} // end namespace itk
  
#endif
