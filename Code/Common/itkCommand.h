/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkCommand.h
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
#ifndef __itkCommand_h
#define __itkCommand_h

#include "itkObject.h"
#include "itkObjectFactory.h"

namespace itk
{

/** \class Command
 * \brief superclass for callback/observer methods
 *
 * Command is an implementation of the command design pattern that is used
 * in callbacks (such as StartMethod(), ProgressMethod(), and EndMethod()) in
 * ITK. itkObject implements a Subject/Observer pattern. When a subject 
 * needs to notify a observer, it does so using a itkCommand.  The Execute 
 * method is called to run the command.
 *
 * \ingroup ITKSystemObjects
 */
  
// The superclass that all commands should be subclasses of
class ITK_EXPORT Command : public Object
{
public:
  /**
   * Standard "Self" typedef.
   */
  typedef Command         Self;

  /**
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  itkTypeMacro(Command,LightObject);
  
  /**
   * Abstract method that defines the action to be taken by the command.
   */
  virtual void Execute(Object *caller, unsigned long event) = 0;

  /**
   * Abstract method that defines the action to be taken by the command.
   * This variant is expected to be used when requests comes from a 
   * const Object
   */
  virtual void Execute(const Object *caller, unsigned long event) = 0;


  /**
   * All the currently defined events are enumerated here.  developers
   * can use: "Command::UserEvent + int" to specify their own events.  
   */
  enum EventIds {
    NoEvent = 0,
    DeleteEvent,
    StartEvent,
    EndEvent,
    ProgressEvent,
    PickEvent,
    StartPickEvent,
    EndPickEvent,
    AbortCheckEvent,
    ExitEvent,
    ModifiedEvent,
    IterationEvent,
    AnyEvent,
    UserEvent = 1000
  };
    
  static unsigned long GetEventIdFromString(const char *event)
    {  
    if (!strcmp("DeleteEvent",event))
      {
      return Command::DeleteEvent;
      }
    if (!strcmp("StartEvent",event))
      {
      return Command::StartEvent;
      }
    if (!strcmp("EndEvent",event))
      {
      return Command::EndEvent;
      }
    if (!strcmp("ProgressEvent",event))
      {
      return Command::ProgressEvent;
      }
    if (!strcmp("PickEvent",event))
      {
      return Command::PickEvent;
      }
    if (!strcmp("ExitEvent",event))
      {
      return Command::PickEvent;
      }
    if (!strcmp("StartPickEvent",event))
      {
      return Command::StartPickEvent;
      }
    if (!strcmp("EndPickEvent",event))
      {
      return Command::EndPickEvent;
      }
    if (!strcmp("AbortCheckEvent",event))
      {
      return Command::AbortCheckEvent;
      }
    if (!strcmp("UserEvent",event))
      {
      return Command::UserEvent;
      }
    return Command::NoEvent;
    };
};
  
// some implementations for several callback types

/** \Class MemberCommand
 *  \brief Command subclass that calls a pointer to a member function
 *
 *  MemberCommand calls a pointer to a member function with the same
 *  arguments as Execute on Command.   
 * 
 * \ingroup ITKSystemObjects
 */
template <class T>
class MemberCommand : public Command
{
public:
  /* 
   * pointer to a member function that takes a Object* and the event
   */
  typedef  void (T::*TMemberFunctionPointer)(Object*, unsigned long);
  typedef  void (T::*TConstMemberFunctionPointer)(const Object*, unsigned long);
  
  /**
   * Standard "Self" typedef.
   */
  typedef MemberCommand         Self;

  /**
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>  Pointer;

  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);
  
  /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro(MemberCommand,Command);

  /**
   *  Set the callback function along with the object that it will
   *  be invoked on.
   */
  void SetCallbackFunction(T* object,  
                           TMemberFunctionPointer memberFunction)
    {
      m_This = object;
      m_MemberFunction = memberFunction;
    }

  void SetCallbackFunction(T* object,  
                           TConstMemberFunctionPointer memberFunction)
    {
      m_This = object;
      m_ConstMemberFunction = memberFunction;
    }

  /**
   *  Invoke the member function.
   */
  virtual void Execute(Object *caller, unsigned long event)
    { 
      if( m_MemberFunction ) 
      {
        ((*m_This).*(m_MemberFunction))(caller, event);
      }
    }

  /**
   *  Invoke the member function with a const object
   */
  virtual void Execute( const Object *caller, unsigned long event)
    { 
      if( m_ConstMemberFunction ) 
      {
        ((*m_This).*(m_ConstMemberFunction))(caller, event);
      }
    }


protected:
  T* m_This;
  TMemberFunctionPointer m_MemberFunction;
  TConstMemberFunctionPointer m_ConstMemberFunction;
  MemberCommand(){
   m_MemberFunction = 0;
   m_ConstMemberFunction = 0;
  }; 
  virtual ~MemberCommand(){}; 
  MemberCommand(const Self&) {}
  void operator=(const Self&) {}
};

/** \Class SimpleMemberCommand
 *  \brief Command subclass that calls a pointer to a member function
 *
 *  SimpleMemberCommand calls a pointer to a member function with no 
 *  arguments.   
 *
 * \ingroup ITKSystemObjects
 */
template <class T>
class SimpleMemberCommand : public Command
{ 
public:
  typedef  void (T::*TMemberFunctionPointer)(); 
   /**
   * Standard "Self" typedef.
   */
  typedef SimpleMemberCommand         Self;

  /**
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>  Pointer;

  /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro(SimpleMemberCommand,Command);

  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);


  void SetCallbackFunction(T* object,  
                           TMemberFunctionPointer memberFunction)
    {
      m_This = object;
      m_MemberFunction = memberFunction;
    }
  
  virtual void Execute(Object *, unsigned long)
    { 
      if( m_MemberFunction ) 
      {
        ((*m_This).*(m_MemberFunction))();
      }
    }

  virtual void Execute(const Object *, unsigned long)
    { 
      if( m_MemberFunction ) 
      {
        ((*m_This).*(m_MemberFunction))();
      }
    }


protected:
  T* m_This;
  TMemberFunctionPointer m_MemberFunction;
  SimpleMemberCommand(){
    m_MemberFunction = 0;
  }; 
  virtual ~SimpleMemberCommand(){}; 
  SimpleMemberCommand(const Self&) {}
  void operator=(const Self&) {}
};


/** \Class SimpleConstMemberCommand
 *  \brief Command subclass that calls a pointer to a member function
 *
 *  SimpleConstMemberCommand calls a pointer to a member function with no 
 *  arguments.   
 *
 * \ingroup ITKSystemObjects
 */
template <class T>
class SimpleConstMemberCommand : public Command
{ 
public:
  typedef  void (T::*TMemberFunctionPointer)() const; 
   /**
   * Standard "Self" typedef.
   */
  typedef SimpleConstMemberCommand         Self;

  /**
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>  Pointer;

  /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro(SimpleConstMemberCommand,Command);

  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);


  void SetCallbackFunction(const T* object,  
                           TMemberFunctionPointer memberFunction)
    {
      m_This = object;
      m_MemberFunction = memberFunction;
    }
  
  virtual void Execute(Object *, unsigned long)
    { 
      if( m_MemberFunction ) 
      {
        ((*m_This).*(m_MemberFunction))();
      }
    }

  virtual void Execute(const Object *, unsigned long)
    { 
      if( m_MemberFunction ) 
      {
        ((*m_This).*(m_MemberFunction))();
      }
    }


protected:
  const T* m_This;
  TMemberFunctionPointer m_MemberFunction;
  SimpleConstMemberCommand(){
    m_MemberFunction = 0;
  }; 
  virtual ~SimpleConstMemberCommand(){}; 
  SimpleConstMemberCommand(const Self&) {}
  void operator=(const Self&) {}
};


/** \Class CStyleCommand
 *  \brief Command subclass that calls a pointer to a C function
 *
 *  CStyleCommand calls a pointer to a C function with the following
 *  arguments void func(Object *,void *clientdata)
 *  The clientdata is data that the command wants passed to itself
 *  each time.
 * 
 * \ingroup ITKSystemObjects
 */

class CStyleCommand : public Command
{
public:
  typedef  void (*FunctionPointer)(Object*, unsigned long, void*);
  typedef  void (*ConstFunctionPointer)(const Object*, unsigned long, void*);
  typedef  void (*DeleteDataFunctionPointer)(void*);

  /**
   * Standard "Self" typedef.
   */
  typedef CStyleCommand         Self;

  /**
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>  Pointer;

  /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro(CStyleCommand,Command);
  
  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);

  /**
   * Set the client data that will be passed into the C function when 
   * it is called.
   */
  void SetClientData(void *cd) {m_ClientData = cd;}

  /**
   * Set the C callback function pointer to be called at Execute time.
   */
  void SetCallback(FunctionPointer f)
    {m_Callback = f;}

  void SetConstCallback(ConstFunctionPointer f)
    {m_ConstCallback = f;}


  /**
   * Set the callback to delete the client data.
   */
  void SetClientDataDeleteCallback(DeleteDataFunctionPointer f)
    {m_ClientDataDeleteCallback = f;}
  
  /**
   * Execute the callback function.
   */
  void Execute(Object *caller, unsigned long event)
    {
    if (m_Callback)
      {
      m_Callback(caller, event, m_ClientData );
      }
    };

  /**
   * Execute the callback function with a const Object
   */
  void Execute(const Object *caller, unsigned long event)
    {
    if (m_ConstCallback)
      {
      m_ConstCallback(caller, event, m_ClientData );
      }
    };


protected:
  CStyleCommand(): m_ClientData(0),
                   m_Callback(0),
                   m_ConstCallback(0),
                   m_ClientDataDeleteCallback(0) {}
  ~CStyleCommand() 
    { 
    if (m_ClientDataDeleteCallback)
      {
      m_ClientDataDeleteCallback(m_ClientData);
      }
    };
  void *m_ClientData;
  FunctionPointer m_Callback;
  ConstFunctionPointer m_ConstCallback;
  DeleteDataFunctionPointer m_ClientDataDeleteCallback;
};


} // end namespace itk

#endif
