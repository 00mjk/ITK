/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkProcessObject.h
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
#ifndef __itkProcessObject_h
#define __itkProcessObject_h

#include "itkObject.h"
#include "itkDataObject.h"
#include "itkMultiThreader.h"
#include "itkObjectFactory.h"
#include <vector>

namespace itk
{

/** \class ProcessObject
 * \brief ProcessObject is the base class for all process objects (source,
 *        filters, mappers) in the Insight data processing pipeline.
 *
 * ProcessObject is an abstract object that specifies behavior and
 * interface of visualization network process objects (sources, filters,
 * mappers). Source objects are creators of visualization data; filters
 * input, process, and output image data; and mappers transform data
 * into another form (like transforming coordinates or writing data to a file).
 *
 * A major role of ProcessObject is to define the inputs and outputs
 * of a filter. More than one input and/or output may exist for a given
 * filter. Some classes (e.g., source objects or mapper objects) will
 * not use inputs (the source) or outputs (mappers). In this case, the
 * inputs or outputs is just ignored.
 *
 * ProcessObject invokes the following events: 
 * Command::StartEvent, Command::EndEvent
 * These are convenience events you can use for any purpose
 * (e.g., debugging info, highlighting/notifying user interface, etc.) 
 * See Command and LightObject for information on using AddObserver.
 *
 * Another event Command::ProgressEvent can be observed. Some filters invoke
 * this event periodically during their execution (with the progress,
 * parameter, the fraction of work done). The use is similar to that of
 * StartEvent and EndEvent. Filters may also check their
 * AbortGenerateData flag to determine whether to prematurally end their
 * execution.
 *
 * An important feature of subclasses of ProcessObject is that it is
 * possible to control the memory-management model (i.e., retain output
 * versus delete output data). If enabled the ReleaseDataFlag enables the
 * deletion of the output data once the downstream process object finishes
 * processing the data (please see text).
 *
 * Subclasses of ProcessObject may override 4 of the methods of this class
 * to control how a given filter may interact with the pipeline (dataflow).
 * These methods are: UpdateOutputInformation(),
 * EnlargeOutputRequestedRegion(), GenerateInputRequestedRegion(), and
 * GenerateOutputRequestedRegion(). By overriding these methods, a filter
 * can deviate from the base assumptions of the pipeline execution model.
 *
 * \ingroup ITKSystemObjects
 * \ingroup DataProcessing 
 *       
 */
class ITK_EXPORT ProcessObject : public Object
{
public:
  /** 
   * Standard "Self" typedef.
   */
  typedef ProcessObject       Self;
  
  /**
   * Standard "Superclass" typedef.
   */
  typedef Object  Superclass;

  /** 
   * Smart pointer typedef support. 
   */
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);

  /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro(ProcessObject,Object);

  /** 
   * Smart Pointer type to a DataObject.
   */
  typedef DataObject::Pointer DataObjectPointer;

  /** 
   * STL Array of SmartPointers to DataObjects
   */
  typedef std::vector<DataObjectPointer> DataObjectPointerArray;

  /** 
   * Return an array with all the inputs of this process object.
   * This is useful for tracing back in the pipeline to construct
   * graphs etc. 
   */
  DataObjectPointerArray GetInputs() 
    {return m_Inputs;}
  unsigned int GetNumberOfInputs() const
    {return m_Inputs.size();}

  /** 
   * Return an array with all the outputs of this process object.
   * This is useful for tracing forward in the pipeline to contruct
   * graphs etc. 
   */
  DataObjectPointerArray GetOutputs()
    { return m_Outputs; }
  unsigned int GetNumberOfOutputs() const
    {return m_Outputs.size();}
    
  /** 
   * Set the AbortGenerateData flag for the process object. Process objects
   *  may handle premature termination of execution in different ways. 
   */
  itkSetMacro(AbortGenerateData,bool);

  /** 
   * Get the AbortGenerateData flag for the process object. Process objects
   *  may handle premature termination of execution in different ways. 
   */
  itkGetConstReferenceMacro(AbortGenerateData,bool);
  
  /**
   * Turn on and off the AbortGenerateData flag.
   */
  itkBooleanMacro(AbortGenerateData); 
  
  /** 
   * Set the execution progress of a process object. The progress is
   * a floating number in [0,1] with 0 meaning no progress and 1 meaning
   * the filter has completed execution.  The ProgressEvent is NOT
   * invoked.
   */
  itkSetClampMacro(Progress,float,0.0,1.0);

  /** 
   * Get the execution progress of a process object. The progress is
   * a floating number in [0,1] with 0 meaning no progress and 1 meaning
   * the filter has completed execution.
   */
  itkGetConstReferenceMacro(Progress,float);

  /** 
   * Update the progress of the process object.
   *
   * Sets the Progress ivar to amount and invokes any observers for
   * the ProgressEvent. The parameter amount should be in [0,1] and is
   * the cumulative (not incremental) progress.
   */
  void UpdateProgress(float amount);
  
  /** 
   * Bring this filter up-to-date. Update() checks modified times against
   * last execution times, and re-executes objects if necessary. A side
   * effect of this method is that the whole pipeline may execute
   * in order to bring this filter up-to-date. This method updates the
   * currently prescribed requested region.  If no requested region has
   * been set on the output, then the requested region will be set to the
   * largest possible region. Once the requested region is set, Update()
   * will make sure the specified requested region is up-to-date. This
   * is a confusing side effect to users who are just calling Update() on
   * a filter.  A first call to Update() will cause the largest possible
   * region to be updated.  A second call to Update() will update that
   * same region.  If a modification to the upstream pipeline cause a
   * filter to have a different largest possible region, this second
   * call to Update() will not cause the output requested region to be
   * reset to the new largest possible region.  Instead, the output requested
   * region will be the same as the last time Update() was called. To have
   * a filter always to produce its largest possible region, users should
   * call UpdateLargestPossibleRegion() instead.
   */
  virtual void Update();

  /** 
   * Like Update(), but sets the output requested region to the
   * largest possible region for the output.  This is the method users
   * should call if they want the entire dataset to be processed.  If
   * a user wants to update the same output region as a previous call
   * to Update() or a previous call to UpdateLargestPossibleRegion(), 
   * then they should call the method Update().
   */
  virtual void UpdateLargestPossibleRegion();

  /** 
   * Update the information decribing the output data. The default 
   * implementation of this method will copy information from the input to
   * the output.  A filter may override this method if its output will have
   * different information than its input.  For instance, a filter that 
   * shrinks an image will need to provide an implementation for this 
   * method that changes the spacing of the pixels. Such filters should call
   * their superclass' implementation of this method prior to changing the
   * information values they need (i.e. UpdateOutputInformation() should
   * call Superclass::UpdateOutputInformation() prior to changing the
   * information.
   */
  virtual void UpdateOutputInformation();

  /** 
   * Send the requested region information back up the pipeline (to the
   * filters that preceed this one).
   */
  virtual void PropagateRequestedRegion(DataObject *output);

  /** 
   * Actually generate new output 
   */
  virtual void UpdateOutputData(DataObject *output);

  /** 
   * Propagate the computation of the size of the pipeline. The first
   * size is the size of the pipeline after this source has finished
   * executing (and potentially freeing some input data). The second
   * size is the size of the specified output. The third size is the
   * maximum pipeline size encountered so far during this propagation.
   * All sizes are in kilobytes. 
   */
  void ComputeEstimatedPipelineMemorySize( DataObject *output,
					   unsigned long size[3] );

  /** 
   * The estimated size of the specified output after execution of
   * this source is stored in the first size entry. The second size
   * is the sum of all estimated output memory. The size of all inputs
   * is given to help this filter in the estimation.
   * All sizes are in kilobytes.
   */
  virtual void ComputeEstimatedOutputMemorySize( DataObject *output,
						 unsigned long *inputSize,
						 unsigned long size[2] );

  /** 
   * Give the process object a chance to indictate that it will produce more
   * output than it was requested to produce. For example, many imaging
   * filters must compute the entire output at once or can only produce output
   * in complete slices. Such filters cannot handle smaller requested regions.
   * These filters must provide an implementation of this method, setting
   * the output requested region to the size they will produce.  By default,
   * a process object does not modify the size of the output requested region.
   */
  virtual void EnlargeOutputRequestedRegion(DataObject *itkNotUsed(output)){};
  
  /** 
   * What is the input requested region that is required to produce the
   * output requested region? By default, the largest possible region is
   * always required but this is overridden in many subclasses. For instance,
   * for an image processing filter where an output pixel is a simple function
   * of an input pixel, the input requested region will be set to the output
   * requested region.  For an image processing filter where an output pixel
   * is a function of the pixels in a neighborhood of an input pixel, then
   * the input requested region will need to be larger than the output
   * requested region (to avoid introducing artificial boundary conditions).
   */
  virtual void GenerateInputRequestedRegion();
  
  /**
   * Given one output whose requested region has been set, how should
   * the requested regions for the remaining outputs of the process object
   * be set?  By default, all the outputs are set to the same requested
   * region.  If a filter needs to produce different requested regions
   * for each output, for instance an image processing filter producing
   * several outputs at different resolutions, then that filter may
   * override this method and set the requested regions appropriatedly.
   */
  virtual void GenerateOutputRequestedRegion(DataObject *output);

  /** 
   * Turn on/off the flags to control whether the data belonging to the
   * outputs of this ProcessObject are released after being used by a
   * source by ProcessObjects further downstream.
   */
  virtual void SetReleaseDataFlag(bool flag);
  virtual bool GetReleaseDataFlag();
  void ReleaseDataFlagOn() {SetReleaseDataFlag(true);}
  void ReleaseDataFlagOff() {SetReleaseDataFlag(false);}

  /**
   * Get/Set the number of threads to create when executing.
   */
  itkSetClampMacro( NumberOfThreads, int, 1, ITK_MAX_THREADS );
  itkGetConstReferenceMacro( NumberOfThreads, int );

  /**
   * Return the multithreader used by this class.
   */
  MultiThreader::Pointer GetMultiThreader()
    {return m_Threader;}
  

protected:
  ProcessObject();
  ~ProcessObject();
  ProcessObject(const ProcessObject&) {};
  void operator=(const ProcessObject&) {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /**
   * Protected methods for setting inputs.
   * Subclasses make use of them for setting input.
   */
  virtual void SetNthInput(unsigned int num, DataObject *input);
  virtual void AddInput(DataObject *input);
  virtual void RemoveInput(DataObject *input);
  itkSetMacro(NumberOfRequiredInputs,unsigned int);
  itkGetConstReferenceMacro(NumberOfRequiredInputs,unsigned int);

  /**
   * Protected methods for setting outputs.
   * Subclasses make use of them for getting output.
   */
  virtual void SetNthOutput(unsigned int num, DataObject *output);
  virtual void AddOutput(DataObject *output);
  virtual void RemoveOutput(DataObject *output);
  itkSetMacro(NumberOfRequiredOutputs,unsigned int);
  itkGetConstReferenceMacro(NumberOfRequiredOutputs,unsigned int);

  /**
   * This method causes the filter to generate its output.
   */
  virtual void GenerateData() {}

  /**
   * Called to allocate the input array.  Copies old inputs.
   */
  void SetNumberOfInputs(unsigned int num);

  /**
   * Method used internally for getting an input.
   */
  DataObjectPointer GetInput(unsigned int idx);

  /**
   * Called to allocate the output array.  Copies old outputs.
   */
  void SetNumberOfOutputs(unsigned int num);

  /**
   * Method used internally for getting an output.
   */
  DataObjectPointer GetOutput(unsigned int idx);

  /**
   * By default, UpdateOutputInformation calls this method to copy information
   * unmodified from the input to the output.
   */
  virtual void GenerateOutputInformation();

  /**
   * These ivars are made protected so filters like itkStreamingImageFilter
   * can access them directly
   *
   *
   *
   */
  
  /**
   * This flag indicates when the pipeline is executing.
   * It prevents infinite recursion when pipelines have loops.
   */
  bool m_Updating;

  /**
   * Time when GenerateOutputInformation was last called.
   */
  TimeStamp m_InformationTime;

 private:
  /**
   * An array of the inputs to the filter.
   */
  std::vector<DataObjectPointer> m_Inputs;
  unsigned int m_NumberOfRequiredInputs;

  /**
   * An array of the outputs to the filter.
   */
  std::vector<DataObjectPointer> m_Outputs;
  unsigned int m_NumberOfRequiredOutputs;

  /**
   * These support the progress method and aborting filter execution.
   */
  bool  m_AbortGenerateData;
  float m_Progress;

  /**
   * Support processing data in multiple threads. Used by subclasses
   * (e.g., ImageSource).
   */
  MultiThreader::Pointer m_Threader;
  int m_NumberOfThreads;

  /**
   * Friends of ProcessObject
   */
  friend class DataObject;
};

} // end namespace itk

#endif

