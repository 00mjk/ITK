/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageSource.txx
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
#ifndef _itkImageSource_txx
#define _itkImageSource_txx
#include "itkImageSource.h"

#include "vnl/vnl_math.h"

namespace itk
{

/**
 *
 */
template<class TOutputImage>
ImageSource<TOutputImage>
::ImageSource()
{
  // Create the output. We use static_cast<> here because we know the default
  // output must be of type TOutputImage
  typename TOutputImage::Pointer output
    = static_cast<TOutputImage*>(this->MakeOutput(0).GetPointer()); 
  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput(0, output.GetPointer());
}

/**
 *
 */
template<class TOutputImage>
ImageSource<TOutputImage>::DataObjectPointer
ImageSource<TOutputImage>
::MakeOutput(unsigned int idx)
{
  return static_cast<DataObject*>(TOutputImage::New().GetPointer());
}
  
/**
 *
 */
template<class TOutputImage>
ImageSource<TOutputImage>::OutputImagePointer 
ImageSource<TOutputImage>
::GetOutput()
{
  if (this->GetNumberOfOutputs() < 1)
    {
    return 0;
    }
  
  return static_cast<TOutputImage*>
                     (this->ProcessObject::GetOutput(0).GetPointer());
}

  
/**
 *
 */
template<class TOutputImage>
ImageSource<TOutputImage>::OutputImagePointer 
ImageSource<TOutputImage>
::GetOutput(unsigned int idx)
{
  return static_cast<TOutputImage*>
                     (this->ProcessObject::GetOutput(idx).GetPointer());
}


/**
 *
 */
template<class TOutputImage>
void 
ImageSource<TOutputImage>
::SetOutput(TOutputImage *output)
{
  itkWarningMacro(<< "SetOutput(): This method is slated to be removed from ITK.  Please use GraftOutput() in possible combination with DisconnectPipeline() instead." );
  this->ProcessObject::SetNthOutput(0, output);
}


/**
 * 
 */
template<class TOutputImage>
void
ImageSource<TOutputImage>
::GraftOutput(TOutputImage *graft)
{
  this->GraftNthOutput(0, graft);
}


/**
 * 
 */
template<class TOutputImage>
void
ImageSource<TOutputImage>
::GraftNthOutput(unsigned int idx, TOutputImage *graft)
{
  if (idx < this->GetNumberOfOutputs())
    {
    OutputImagePointer output = this->GetOutput(idx);

    if (output && graft)
      {
      // grab a handle to the bulk data of the specified data object
      output->SetPixelContainer( graft->GetPixelContainer() );
      
      // copy the region ivars of the specified data object
      output->SetRequestedRegion( graft->GetRequestedRegion() );
      output->SetLargestPossibleRegion( graft->GetLargestPossibleRegion() );
      output->SetBufferedRegion( graft->GetBufferedRegion() );
      
      // copy the meta-information
      output->CopyInformation( graft );
      }
    }
}


/**
 *
 */
template<class TOutputImage>
void 
ImageSource<TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
template <class TOutputImage>
int 
ImageSource<TOutputImage>
::SplitRequestedRegion(int i, int num, OutputImageRegionType& splitRegion)
{
  // Get the output pointer
  OutputImagePointer outputPtr = this->GetOutput();
  const typename TOutputImage::SizeType& requestedRegionSize 
    = outputPtr->GetRequestedRegion().GetSize();

  int splitAxis;
  typename TOutputImage::IndexType splitIndex;
  typename TOutputImage::SizeType splitSize;

  // Initialize the splitRegion to the output requested region
  splitRegion = outputPtr->GetRequestedRegion();
  splitIndex = splitRegion.GetIndex();
  splitSize = splitRegion.GetSize();

  // split on the outermost dimension available
  splitAxis = outputPtr->GetImageDimension() - 1;
  while (requestedRegionSize[splitAxis] == 1)
    {
    --splitAxis;
    if (splitAxis < 0)
      { // cannot split
      itkDebugMacro("  Cannot Split");
      return 1;
      }
    }

  // determine the actual number of pieces that will be generated
  typename TOutputImage::SizeType::SizeValueType range = requestedRegionSize[splitAxis];
  int valuesPerThread = (int)ceil(range/(double)num);
  int maxThreadIdUsed = (int)ceil(range/(double)valuesPerThread) - 1;

  // Split the region
  if (i < maxThreadIdUsed)
    {
    splitIndex[splitAxis] += i*valuesPerThread;
    splitSize[splitAxis] = valuesPerThread;
    }
  if (i == maxThreadIdUsed)
    {
    splitIndex[splitAxis] += i*valuesPerThread;
    // last thread needs to process the "rest" dimension being split
    splitSize[splitAxis] = splitSize[splitAxis] - i*valuesPerThread;
    }
  
  // set the split region ivars
  splitRegion.SetIndex( splitIndex );
  splitRegion.SetSize( splitSize );

  itkDebugMacro("  Split Piece: " << splitRegion );

  return maxThreadIdUsed + 1;
}

//----------------------------------------------------------------------------
template <class TOutputImage>
void 
ImageSource<TOutputImage>
::GenerateData()
{
  OutputImagePointer outputPtr;

  // Allocate the output memory
  for (unsigned int i=0; i < this->GetNumberOfOutputs(); i++)
    {
    outputPtr = this->GetOutput(i);
    outputPtr->SetBufferedRegion(outputPtr->GetRequestedRegion());
    outputPtr->Allocate();
    }

  // Call a method that can be overridden by a subclass to perform
  // some calculations prior to splitting the main computations into
  // separate threads
  this->BeforeThreadedGenerateData();
  
  // Set up the multithreaded processing
  ThreadStruct str;
  str.Filter = this;
  
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ThreaderCallback, &str);
  
  // multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  // Call a method that can be overridden by a subclass to perform
  // some calculations after all the threads have completed
  this->AfterThreadedGenerateData();
}


//----------------------------------------------------------------------------
// The execute method created by the subclass.
template <class TOutputImage>
void 
ImageSource<TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId )
{
  itkErrorMacro("subclass should override this method!!!");
}

// Callback routine used by the threading library. This routine just calls
// the ThreadedGenerateData method after setting the correct region for this
// thread. 
template <class TOutputImage>
ITK_THREAD_RETURN_TYPE 
ImageSource<TOutputImage>
::ThreaderCallback( void *arg )
{
  ThreadStruct *str;
  int total, threadId, threadCount;

  threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;
  threadCount = ((MultiThreader::ThreadInfoStruct *)(arg))->NumberOfThreads;

  str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  // execute the actual method with appropriate output region
  // first find out how many pieces extent can be split into.
  typename TOutputImage::RegionType splitRegion;
  total = str->Filter->SplitRequestedRegion(threadId, threadCount,
                                            splitRegion);

  if (threadId < total)
    {
    str->Filter->ThreadedGenerateData(splitRegion, threadId);
    }
  // else
  //   {
  //   otherwise don't use this thread. Sometimes the threads dont
  //   break up very well and it is just as efficient to leave a 
  //   few threads idle.
  //   }
  
  return ITK_THREAD_RETURN_VALUE;
}


} // end namespace itk

#endif
