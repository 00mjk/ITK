/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkMultiThreader.cxx
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

#include "itkMultiThreader.h"
#include "itkObjectFactory.h"
#ifndef _WIN32
#include <unistd.h>
#endif
#ifdef _WIN32
#include "itkWindows.h"
// when we start using _beginthreadex() instead of CreateThread we'll need
// this include file
//#include <process.h>
#endif
// These are the includes necessary for multithreaded rendering on an SGI
// using the sproc() call
#ifdef ITK_USE_SPROC
#include <sys/resource.h>
#include <sys/prctl.h>
#include <wait.h>
#include <errno.h>
#endif


namespace itk {

// Initialize static member that controls global maximum number of threads
int MultiThreader::m_GlobalMaximumNumberOfThreads = 0;
// 0 => Not initialized.
int MultiThreader::m_GlobalDefaultNumberOfThreads = 0;

void MultiThreader::SetGlobalMaximumNumberOfThreads(int val)
{
  if (val == m_GlobalMaximumNumberOfThreads)
    {
    return;
    }
  m_GlobalMaximumNumberOfThreads = val;
}

int MultiThreader::GetGlobalMaximumNumberOfThreads()
{
  return m_GlobalMaximumNumberOfThreads;
}


void MultiThreader::SetGlobalDefaultNumberOfThreads(int val)
{
  if (val == m_GlobalDefaultNumberOfThreads)
    {
    return;
    }
  m_GlobalDefaultNumberOfThreads = val;
}

int MultiThreader::GetGlobalDefaultNumberOfThreads()
{
  if (m_GlobalDefaultNumberOfThreads == 0)
    {
    int num;
#ifdef ITK_USE_SPROC
    // Default the number of threads to be the number of available
    // processors if we are using sproc()
    num = prctl( PR_MAXPPROCS );
#endif

#ifdef ITK_USE_PTHREADS
    // Default the number of threads to be the number of available
    // processors if we are using pthreads()
#ifdef _SC_NPROCESSORS_ONLN
    num = sysconf( _SC_NPROCESSORS_ONLN );
#elif defined(_SC_NPROC_ONLN)
    num = sysconf( _SC_NPROC_ONLN );
#else
    num = 1;
#endif
#if defined(__SVR4) && defined(sun)
    pthread_setconcurrency(num);
#endif
#endif

#if defined(_WIN32)
    {
      SYSTEM_INFO sysInfo;
      GetSystemInfo(&sysInfo);
      num = sysInfo.dwNumberOfProcessors;
    }
#endif

#ifndef ITK_USE_WIN32_THREADS
#ifndef ITK_USE_SPROC
#ifndef ITK_USE_PTHREADS
    // If we are not multithreading, the number of threads should
    // always be 1
    num = 1;
#endif  
#endif  
#endif
  
    // Limit the number of threads
    if (num > ITK_MAX_THREADS)
      {
      num = ITK_MAX_THREADS;
      }
    m_GlobalDefaultNumberOfThreads = num;
    }

  return m_GlobalDefaultNumberOfThreads;
}

// Constructor. Default all the methods to NULL. Since the
// ThreadInfoArray is static, the ThreadIDs can be initialized here
// and will not change.
MultiThreader::MultiThreader()
{
  int i;

  for ( i = 0; i < ITK_MAX_THREADS; i++ )
    {
    m_ThreadInfoArray[i].ThreadID           = i;
    m_ThreadInfoArray[i].ActiveFlag         = NULL;
    m_ThreadInfoArray[i].ActiveFlagLock     = 0;
    m_MultipleMethod[i]                     = NULL;
    m_MultipleData[i]                       = NULL;
    m_SpawnedThreadActiveFlag[i]            = 0;
    m_SpawnedThreadActiveFlagLock[i]        = 0;
    m_SpawnedThreadInfoArray[i].ThreadID    = i;
    }

  m_SingleMethod = NULL;
  m_SingleData = NULL;
  m_NumberOfThreads = this->GetGlobalDefaultNumberOfThreads();
}

// Destructor. Nothing allocated so nothing needs to be done here.
MultiThreader::~MultiThreader()
{
}

// Set the user defined method that will be run on NumberOfThreads threads
// when SingleMethodExecute is called.
void MultiThreader::SetSingleMethod( ThreadFunctionType f, 
          void *data )
{ 
  m_SingleMethod = f;
  m_SingleData   = data;
}

// Set one of the user defined methods that will be run on NumberOfThreads
// threads when MultipleMethodExecute is called. This method should be
// called with index = 0, 1, ..,  NumberOfThreads-1 to set up all the
// required user defined methods
void MultiThreader::SetMultipleMethod( int index, 
            ThreadFunctionType f, void *data )
{ 
  // You can only set the method for 0 through NumberOfThreads-1
  if ( index >= m_NumberOfThreads ) {
    itkExceptionMacro( << "Can't set method " << index << 
    " with a thread count of " << m_NumberOfThreads );
    }
  else
    {
    m_MultipleMethod[index] = f;
    m_MultipleData[index]   = data;
    }
}

// Execute the method set as the SingleMethod on NumberOfThreads threads.
void MultiThreader::SingleMethodExecute()
{
  int                thread_loop = 0;

#ifdef ITK_USE_WIN32_THREADS
  DWORD              threadId;
  HANDLE             process_id[ITK_MAX_THREADS];
#endif

#ifdef ITK_USE_SPROC
  siginfo_t          info_ptr;
  int                process_id[ITK_MAX_THREADS];
#endif

#ifdef ITK_USE_PTHREADS
  pthread_t          process_id[ITK_MAX_THREADS];
#endif

  if ( !m_SingleMethod )
    {
    itkExceptionMacro( << "No single method set!" );
    return;
    }

  // obey the global maximum number of threads limit
  if (m_GlobalMaximumNumberOfThreads &&
      m_NumberOfThreads > m_GlobalMaximumNumberOfThreads)
    {
    m_NumberOfThreads = m_GlobalMaximumNumberOfThreads;
    }
  
    
  // We are using sproc (on SGIs), pthreads(on Suns), or a single thread
  // (the default)  

#ifdef ITK_USE_WIN32_THREADS
  // Using CreateThread on a PC
  //
  // We want to use CreateThread to start m_NumberOfThreads - 1 
  // additional threads which will be used to call this->SingleMethod().
  // The parent thread will also call this routine.  When it is done,
  // it will wait for all the children to finish. 
  //
  // First, start up the m_NumberOfThreads-1 processes.  Keep track
  // of their process ids for use later in the waitid call
  for (thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    m_ThreadInfoArray[thread_loop].UserData    = m_SingleData;
    m_ThreadInfoArray[thread_loop].NumberOfThreads = m_NumberOfThreads;
    process_id[thread_loop] = 
      CreateThread(NULL, 0, m_SingleMethod, 
                   ((void *)(&m_ThreadInfoArray[thread_loop])), 0, &threadId);
// Found an article that said we should not use CreateThread but should
// _beginthreadex() so that the CRT and exceptions are handled properly.
// Not quite ready to try this.
//    
//     process_id[thread_loop] = (void *)
//       _beginthreadex(NULL, 0,
//                      (unsigned int (__stdcall *)(void *))m_SingleMethod, 
//                      ((void *)(&m_ThreadInfoArray[thread_loop])), 0,
//                      (unsigned int *)&threadId);
    if (process_id == NULL)
      {
      itkExceptionMacro("Error in thread creation !!!");
      } 
    }
  
  // Now, the parent thread calls this->SingleMethod() itself
  m_ThreadInfoArray[0].UserData = m_SingleData;
  m_ThreadInfoArray[0].NumberOfThreads = m_NumberOfThreads;
  m_SingleMethod((void *)(&m_ThreadInfoArray[0]));

  // The parent thread has finished this->SingleMethod() - so now it
  // waits for each of the other processes to exit
  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    WaitForSingleObject(process_id[thread_loop], INFINITE);
    }

  // close the threads
  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    CloseHandle(process_id[thread_loop]);
    }
#endif

#ifdef ITK_USE_SPROC
  // Using sproc() on an SGI
  //
  // We want to use sproc to start m_NumberOfThreads - 1 additional
  // threads which will be used to call this->SingleMethod(). The
  // parent thread will also call this routine.  When it is done,
  // it will wait for all the children to finish. 
  //
  // First, start up the m_NumberOfThreads-1 processes.  Keep track
  // of their process ids for use later in the waitid call

  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    m_ThreadInfoArray[thread_loop].UserData    = m_SingleData;
    m_ThreadInfoArray[thread_loop].NumberOfThreads = m_NumberOfThreads;
    process_id[thread_loop] = 
      sproc( m_SingleMethod, PR_SADDR, 
       ( (void *)(&m_ThreadInfoArray[thread_loop]) ) );
    if ( process_id[thread_loop] == -1)
      {
      itkExceptionMacro("sproc call failed. Code: " << errno << std::endl);
      }
    }
  
  // Now, the parent thread calls this->SingleMethod() itself
  m_ThreadInfoArray[0].UserData = m_SingleData;
  m_ThreadInfoArray[0].NumberOfThreads = m_NumberOfThreads;
  m_SingleMethod( (void *)(&m_ThreadInfoArray[0]) );

  // The parent thread has finished this->SingleMethod() - so now it
  // waits for each of the other processes to exit
  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    waitid( P_PID, (id_t) process_id[thread_loop], &info_ptr, WEXITED );
    }
#endif

#ifdef ITK_USE_PTHREADS
  // Using POSIX threads
  //
  // We want to use pthread_create to start m_NumberOfThreads-1 additional
  // threads which will be used to call this->SingleMethod(). The
  // parent thread will also call this routine.  When it is done,
  // it will wait for all the children to finish. 
  //
  // First, start up the m_NumberOfThreads-1 processes.  Keep track
  // of their process ids for use later in the pthread_join call

  pthread_attr_t attr;

#ifdef ITK_HP_PTHREADS
  pthread_attr_create( &attr );
#else  
  pthread_attr_init(&attr);
#if !defined(__CYGWIN__)
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
#endif
#endif
  
  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    m_ThreadInfoArray[thread_loop].UserData    = m_SingleData;
    m_ThreadInfoArray[thread_loop].NumberOfThreads = m_NumberOfThreads;

#ifdef ITK_HP_PTHREADS
    pthread_create( &(process_id[thread_loop]),
        attr, m_SingleMethod,  
        ( (void *)(&m_ThreadInfoArray[thread_loop]) ) );
#else
    int                threadError;
    threadError =
      pthread_create( &(process_id[thread_loop]), &attr, m_SingleMethod,  
          ( (void *)(&m_ThreadInfoArray[thread_loop]) ) );
    if (threadError != 0)
      {
      itkExceptionMacro(<< "Unable to create a thread.  pthread_create() returned "
                    << threadError);
      }
#endif
    }
  
  // Now, the parent thread calls this->SingleMethod() itself
  m_ThreadInfoArray[0].UserData = m_SingleData;
  m_ThreadInfoArray[0].NumberOfThreads = m_NumberOfThreads;
  m_SingleMethod((void *)(&m_ThreadInfoArray[0]) );

  // The parent thread has finished this->SingleMethod() - so now it
  // waits for each of the other processes to exit
  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    if ( pthread_join( process_id[thread_loop], NULL ) )
      {
      itkExceptionMacro(<< "Unable to join thread " << thread_loop);
      }
    }
#endif

#ifndef ITK_USE_WIN32_THREADS
#ifndef ITK_USE_SPROC
#ifndef ITK_USE_PTHREADS
  // There is no multi threading, so there is only one thread.
  m_ThreadInfoArray[0].UserData    = m_SingleData;
  m_ThreadInfoArray[0].NumberOfThreads = m_NumberOfThreads;
  m_SingleMethod( (void *)(&m_ThreadInfoArray[0]) );
#endif
#endif
#endif
}

void MultiThreader::MultipleMethodExecute()
{
  int                thread_loop;

#ifdef ITK_USE_WIN32_THREADS
  DWORD              threadId;
  HANDLE             process_id[ITK_MAX_THREADS];
#endif

#ifdef ITK_USE_SPROC
  siginfo_t          info_ptr;
  int                process_id[ITK_MAX_THREADS];
#endif

#ifdef ITK_USE_PTHREADS
  pthread_t          process_id[ITK_MAX_THREADS];
#endif


  // obey the global maximum number of threads limit
  if (m_GlobalMaximumNumberOfThreads &&
      m_NumberOfThreads > m_GlobalMaximumNumberOfThreads)
    {
    m_NumberOfThreads = m_GlobalMaximumNumberOfThreads;
    }

  for ( thread_loop = 0; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    if ( m_MultipleMethod[thread_loop] == (ThreadFunctionType)NULL)
      {
      itkExceptionMacro( << "No multiple method set for: " << thread_loop );
      return;
      }
    }

  // We are using sproc (on SGIs), pthreads(on Suns), CreateThread
  // on a PC or a single thread (the default)  

#ifdef ITK_USE_WIN32_THREADS
  // Using CreateThread on a PC
  //
  // We want to use CreateThread to start m_NumberOfThreads - 1 
  // additional threads which will be used to call the NumberOfThreads-1
  // methods defined in this->MultipleMethods[](). The parent thread
  // will call m_MultipleMethods[NumberOfThreads-1]().  When it is done,
  // it will wait for all the children to finish. 
  //
  // First, start up the m_NumberOfThreads-1 processes.  Keep track
  // of their process ids for use later in the waitid call
  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    m_ThreadInfoArray[thread_loop].UserData = 
      m_MultipleData[thread_loop];
    m_ThreadInfoArray[thread_loop].NumberOfThreads = m_NumberOfThreads;
    process_id[thread_loop] = 
      CreateThread(NULL, 0, m_MultipleMethod[thread_loop], 
       ((void *)(&m_ThreadInfoArray[thread_loop])), 0, &threadId);
    if (process_id == NULL)
      {
      itkExceptionMacro("Error in thread creation !!!");
      } 
    }
  
  // Now, the parent thread calls the last method itself
  m_ThreadInfoArray[0].UserData = m_MultipleData[0];
  m_ThreadInfoArray[0].NumberOfThreads = m_NumberOfThreads;
  (m_MultipleMethod[0])((void *)(&m_ThreadInfoArray[0]) );

  // The parent thread has finished its method - so now it
  // waits for each of the other processes (created with sproc) to
  // exit
  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    WaitForSingleObject(process_id[thread_loop], INFINITE);
    }

  // close the threads
  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    CloseHandle(process_id[thread_loop]);
    }
#endif

#ifdef ITK_USE_SPROC
  // Using sproc() on an SGI
  //
  // We want to use sproc to start m_NumberOfThreads - 1 additional
  // threads which will be used to call the NumberOfThreads-1 methods
  // defined in m_MultipleMethods[](). The parent thread
  // will call m_MultipleMethods[NumberOfThreads-1]().  When it is done,
  // it will wait for all the children to finish. 
  //
  // First, start up the m_NumberOfThreads-1 processes.  Keep track
  // of their process ids for use later in the waitid call
  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    m_ThreadInfoArray[thread_loop].UserData = 
      m_MultipleData[thread_loop];
    m_ThreadInfoArray[thread_loop].NumberOfThreads = m_NumberOfThreads;
    process_id[thread_loop] = 
      sproc( m_MultipleMethod[thread_loop], PR_SADDR, 
       ( (void *)(&m_ThreadInfoArray[thread_loop]) ) );
    }
  
  // Now, the parent thread calls the last method itself
  m_ThreadInfoArray[0].UserData = m_MultipleData[0];
  m_ThreadInfoArray[0].NumberOfThreads = m_NumberOfThreads;
  (m_MultipleMethod[0])((void *)(&m_ThreadInfoArray[0]) );

  // The parent thread has finished its method - so now it
  // waits for each of the other processes (created with sproc) to
  // exit
  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    waitid( P_PID, (id_t) process_id[thread_loop], &info_ptr, WEXITED );
    }
#endif

#ifdef ITK_USE_PTHREADS
  // Using POSIX threads
  //
  // We want to use pthread_create to start m_NumberOfThreads - 1 
  // additional
  // threads which will be used to call the NumberOfThreads-1 methods
  // defined in m_MultipleMethods[](). The parent thread
  // will call m_MultipleMethods[NumberOfThreads-1]().  When it is done,
  // it will wait for all the children to finish. 
  //
  // First, start up the m_NumberOfThreads-1 processes.  Keep track
  // of their process ids for use later in the pthread_join call

  pthread_attr_t attr;

#ifdef ITK_HP_PTHREADS
  pthread_attr_create( &attr );
#else  
  pthread_attr_init(&attr);
#ifndef __CYGWIN__
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
#endif
#endif

  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    m_ThreadInfoArray[thread_loop].UserData = 
      m_MultipleData[thread_loop];
    m_ThreadInfoArray[thread_loop].NumberOfThreads = m_NumberOfThreads;
#ifdef ITK_HP_PTHREADS
    pthread_create( &(process_id[thread_loop]),
        attr, m_MultipleMethod[thread_loop],  
        ( (void *)(&m_ThreadInfoArray[thread_loop]) ) );
#else
    pthread_create( &(process_id[thread_loop]),
        &attr, m_MultipleMethod[thread_loop],  
        ( (void *)(&m_ThreadInfoArray[thread_loop]) ) );
#endif
    }
  
  // Now, the parent thread calls the last method itself
  m_ThreadInfoArray[0].UserData = m_MultipleData[0];
  m_ThreadInfoArray[0].NumberOfThreads = m_NumberOfThreads;
  (m_MultipleMethod[0])((void *)(&m_ThreadInfoArray[0]) );

  // The parent thread has finished its method - so now it
  // waits for each of the other processes to exit
  for ( thread_loop = 1; thread_loop < m_NumberOfThreads; thread_loop++ )
    {
    pthread_join( process_id[thread_loop], NULL );
    }
#endif

#ifndef ITK_USE_WIN32_THREADS
#ifndef ITK_USE_SPROC
#ifndef ITK_USE_PTHREADS
  // There is no multi threading, so there is only one thread.
  m_ThreadInfoArray[0].UserData    = m_MultipleData[0];
  m_ThreadInfoArray[0].NumberOfThreads = m_NumberOfThreads;
  (m_MultipleMethod[0])( (void *)(&m_ThreadInfoArray[0]) );
#endif
#endif
#endif
}

int MultiThreader::SpawnThread( ThreadFunctionType f, void *UserData )
{
  int id;

  // avoid a warning
  ThreadFunctionType tf;
  tf = f; tf= tf;
  
#ifdef ITK_USE_WIN32_THREADS
  DWORD              threadId;
#endif

  id = 0;

  while ( id < ITK_MAX_THREADS )
    {
    if ( ! m_SpawnedThreadActiveFlagLock[id]  )
      {
      m_SpawnedThreadActiveFlagLock[id] = MutexLock::New();
      }
    m_SpawnedThreadActiveFlagLock[id]->Lock();
    if (m_SpawnedThreadActiveFlag[id] == 0)
      {
      // We've got a useable thread id, so grab it
      m_SpawnedThreadActiveFlag[id] = 1;
      m_SpawnedThreadActiveFlagLock[id]->Unlock();
      break;
      }
    m_SpawnedThreadActiveFlagLock[id]->Unlock();
      
    id++;
    }

  if ( id >= ITK_MAX_THREADS )
    {
    itkExceptionMacro( << "You have too many active threads!" );
    return -1;
    }


  m_SpawnedThreadInfoArray[id].UserData        = UserData;
  m_SpawnedThreadInfoArray[id].NumberOfThreads = 1;
  m_SpawnedThreadInfoArray[id].ActiveFlag = &m_SpawnedThreadActiveFlag[id];
  m_SpawnedThreadInfoArray[id].ActiveFlagLock =
    m_SpawnedThreadActiveFlagLock[id];

  // We are using sproc (on SGIs), pthreads(on Suns or HPs), 
  // CreateThread (on win32), or generating an error  

#ifdef ITK_USE_WIN32_THREADS
  // Using CreateThread on a PC
  //
  m_SpawnedThreadProcessID[id] = 
      CreateThread(NULL, 0, f, 
       ((void *)(&m_SpawnedThreadInfoArray[id])), 0, &threadId);
  if (m_SpawnedThreadProcessID[id] == NULL)
    {
    itkExceptionMacro("Error in thread creation !!!");
    } 
#endif

#ifdef ITK_USE_SPROC
  // Using sproc() on an SGI
  //
  m_SpawnedThreadProcessID[id] = 
    sproc( f, PR_SADDR, ( (void *)(&m_SpawnedThreadInfoArray[id]) ) );

#endif

#ifdef ITK_USE_PTHREADS
  // Using POSIX threads
  //
  pthread_attr_t attr;

#ifdef ITK_HP_PTHREADS
  pthread_attr_create( &attr );
#else  
  pthread_attr_init(&attr);
#ifndef __CYGWIN__
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
#endif
#endif
  
#ifdef ITK_HP_PTHREADS
  pthread_create( &(m_SpawnedThreadProcessID[id]),
      attr, f,  
      ( (void *)(&m_SpawnedThreadInfoArray[id]) ) );
#else
  pthread_create( &(m_SpawnedThreadProcessID[id]),
      &attr, f,  
      ( (void *)(&m_SpawnedThreadInfoArray[id]) ) );
#endif

#endif

#ifndef ITK_USE_WIN32_THREADS
#ifndef ITK_USE_SPROC
#ifndef ITK_USE_PTHREADS
  // There is no multi threading, so there is only one thread.
  // This won't work - so give an error message.
  itkExceptionMacro( << "Cannot spawn thread in a single threaded environment!" );
  m_SpawnedThreadActiveFlagLock[id] = 0;
  id = -1;
#endif
#endif
#endif

  return id;
}

void MultiThreader::TerminateThread( int ThreadID )
{

  if ( !m_SpawnedThreadActiveFlag[ThreadID] ) {
    return;
  }

  m_SpawnedThreadActiveFlagLock[ThreadID]->Lock();
  m_SpawnedThreadActiveFlag[ThreadID] = 0;
  m_SpawnedThreadActiveFlagLock[ThreadID]->Unlock();

#ifdef ITK_USE_WIN32_THREADS
  WaitForSingleObject(m_SpawnedThreadProcessID[ThreadID], INFINITE);
  CloseHandle(m_SpawnedThreadProcessID[ThreadID]);
#endif

#ifdef ITK_USE_SPROC
  siginfo_t info_ptr;

  waitid( P_PID, (id_t) m_SpawnedThreadProcessID[ThreadID], 
    &info_ptr, WEXITED );
#endif

#ifdef ITK_USE_PTHREADS
  pthread_join( m_SpawnedThreadProcessID[ThreadID], NULL );
#endif

#ifndef ITK_USE_WIN32_THREADS
#ifndef ITK_USE_SPROC
#ifndef ITK_USE_PTHREADS
  // There is no multi threading, so there is only one thread.
  // This won't work - so give an error message.
  itkExceptionMacro(<< "Cannot terminate thread in single threaded environment!");
#endif
#endif
#endif

  m_SpawnedThreadActiveFlagLock[ThreadID] = 0;
  m_SpawnedThreadActiveFlagLock[ThreadID] = NULL;

}

// Print method for the multithreader
void MultiThreader::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent); 

  os << indent << "Thread Count: " << m_NumberOfThreads << "\n";
  os << indent << "Global Maximum Number Of Threads: " << 
    m_GlobalMaximumNumberOfThreads << std::endl;

}

} // end namespace itk
