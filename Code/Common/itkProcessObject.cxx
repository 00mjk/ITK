/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkProcessObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.

=========================================================================*/
#include "itkProcessObject.h"
#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "itkCommand.h"
#include <algorithm>

namespace itk
{

/**
 * Instantiate object with no start, end, or progress methods.
 */
ProcessObject
::ProcessObject()
{
  m_NumberOfRequiredInputs = 0;

  m_NumberOfRequiredOutputs = 0;

  m_AbortGenerateData = false;
  m_Progress = 0.0;
  m_Updating = false;
  
  m_Threader = MultiThreader::New();
  m_NumberOfThreads = m_Threader->GetNumberOfThreads();
}


/**
 * Destructor for the ProcessObject class. We've got to
 * UnRegister() the use of any input classes.
 */
ProcessObject
::~ProcessObject()
{
  // Tell each output that we are going away.  If other objects have a 
  // reference to one of these outputs, the data object will not be deleted
  // when the process object is deleted.  However, the data object's source
  // will still point back to the now nonexistent process object if we do not
  // clean things up now.
  int idx;
  for (idx = 0; idx < m_Outputs.size(); ++idx)
    {
    if (m_Outputs[idx])
      {
      // let the output know we no longer want to associate with the object
      m_Outputs[idx]->DisconnectSource(this, idx);
      // let go of our reference to the data object
      m_Outputs[idx] = 0;
      }
    }
}

typedef DataObject *DataObjectPointer;

/**
 * Called by constructor to set up input array.
 */
void 
ProcessObject
::SetNumberOfInputs(unsigned int num)
{
  // in case nothing has changed.
  if (num == m_Inputs.size())
    {
    return;
    }
  m_Inputs.resize(num);
  this->Modified();
}


/**
 * Adds an input to the first null position in the input list.
 * Expands the list memory if necessary
 */
void 
ProcessObject
::AddInput(DataObject *input)
{
  std::vector<DataObjectPointer>::size_type idx;
  
  this->Modified();
  
  for (idx = 0; idx < m_Inputs.size(); ++idx)
    {
    if (!m_Inputs[idx])
      {
      m_Inputs[idx] = input;
      return;
      }
    }
  
  this->SetNumberOfInputs(m_Inputs.size() + 1);
  m_Inputs[m_Inputs.size() - 1] = input;
}


/**
 * Adds an input to the first null position in the input list.
 * Expands the list memory if necessary
 */
void 
ProcessObject
::RemoveInput(DataObject *input)
{
  if (!input)
    {
    return;
    }
  
  // find the input in the list of inputs
  DataObjectPointerArray::iterator pos = 
    std::find(m_Inputs.begin(), m_Inputs.end(), input);

  if(pos == m_Inputs.end())
    {
    itkDebugMacro("tried to remove an input that was not in the list");
    return;
    }

  // Set the position in the m_Inputs containing input to 0
  *pos = 0;

  // if that was the last input, then shrink the list
  if (pos == m_Inputs.end() - 1 )
    {
    this->SetNumberOfInputs(m_Inputs.size() - 1);
    }

  this->Modified();
}


/**
 * Set an Input of this filter. This method 
 * does Register()/UnRegister() manually to
 * deal with the fact that smart pointers aren't
 * around to do the reference counting.
 */
void 
ProcessObject
::SetNthInput(unsigned int idx, DataObject *input)
{
  // does this change anything?
  if ( idx < m_Inputs.size() && m_Inputs[idx] == input )
    {
    return;
    }
  
  // Expand array if necessary.
  if (idx >= m_Inputs.size())
    {
    this->SetNumberOfInputs(idx + 1);
    }
  
  m_Inputs[idx] = input;

  this->Modified();
}

void 
ProcessObject
::RemoveOutput(DataObject *output)
{
  if (!output)
    {
    return;
    }
  
  // find the input in the list of inputs
  DataObjectPointerArray::iterator pos = 
    std::find(m_Outputs.begin(), m_Outputs.end(), output);

  if(pos == m_Outputs.end())
    {
    itkDebugMacro("tried to remove an output that was not in the list");
    return;
    }

  // let the output know we no longer want to associate with the object
  (*pos)->DisconnectSource(this, pos - m_Outputs.begin());
  // let go of our reference to the data object
  *pos = 0;

  // if that was the last output, then shrink the list
  if (pos == m_Outputs.end() - 1 )
    {
    this->SetNumberOfOutputs(m_Outputs.size() - 1);
    }

  this->Modified();
}


/**
 * Set an output of this filter. This method specifically
 * does not do a Register()/UnRegister() because of the 
 * desire to break the reference counting loop.
 */
void 
ProcessObject
::SetNthOutput(unsigned int idx, DataObject *output)
{
  // does this change anything?
  if ( idx < m_Outputs.size() && output == m_Outputs[idx])
    {
    return;
    }

  // Expand array if necessary.
  if (idx >= m_Outputs.size())
    {
    this->SetNumberOfOutputs(idx + 1);
    }

  if ( m_Outputs[idx] )
    {
    m_Outputs[idx]->DisconnectSource(this, idx);
    }

  if (output)
    {
    output->ConnectSource(this, idx);
    }
  // save the current reference (which releases the previous reference)
  m_Outputs[idx] = output;

  // if we are clearing an output, we need to create a new blank output
  // so we are prepared for the next Update()
//   if (!m_Outputs[idx])
//     {
//     itkDebugMacro( << this->GetClassName() << " (" 
//                    << this << "): creating new output object." );
//     DataObjectPointer newOutput = ...
//     this->SetNthOutput(idx, newOutput);
//     }

  this->Modified();
}

/**
 * Adds an output to the first null position in the output list.
 * Expands the list memory if necessary
 */
void 
ProcessObject
::AddOutput(DataObject *output)
{
  unsigned int idx;
  
  for (idx = 0; idx < m_Outputs.size(); ++idx)
    {
    if (m_Outputs[idx] == 0)
      {
      m_Outputs[idx] = output;

      if (output)
        {
        output->ConnectSource(this, idx);
        }
      this->Modified();
  
      return;
      }
    }
  
  this->SetNumberOfOutputs(m_Outputs.size() + 1);
  m_Outputs[m_Outputs.size() - 1] = output;
  if (output)
    {
    output->ConnectSource(this, m_Outputs.size()-1);
    }
  this->Modified();
}

/**
 * Called by constructor to set up output array.
 */
void 
ProcessObject
::SetNumberOfOutputs(unsigned int num)
{

  // in case nothing has changed.
  if (num == m_Outputs.size())
    {
    return;
    }
  m_Outputs.resize(num);
  this->Modified();
}


/**
 *
 */
DataObject::Pointer
ProcessObject
::GetOutput(unsigned int i)
{
  if (m_Outputs.size() < i+1)
    {
    return NULL;
    }
  
  return m_Outputs[i];
}


/**
 *
 */
DataObject::Pointer
ProcessObject
::GetInput(unsigned int i)
{
  if (m_Inputs.size() < i+1)
    {
    return NULL;
    }
  
  return m_Inputs[i];
}


/**
 * Update the progress of the process object. If a ProgressMethod exists, 
 * executes it. Then set the Progress ivar to amount. The parameter amount 
 * should range between (0,1).
 */
void 
ProcessObject
::UpdateProgress(float amount)
{
  m_Progress = amount;
  this->InvokeEvent(Command::ProgressEvent);
}


/**
 *
 */
bool 
ProcessObject
::GetReleaseDataFlag()
{
  if (this->GetOutput(0))
    {
    return this->GetOutput(0)->GetReleaseDataFlag();
    }
  itkWarningMacro(<<"Output doesn't exist!");
  return false;
}


/**
 *
 */
void 
ProcessObject
::SetReleaseDataFlag(bool val)
{
  unsigned int idx;
  
  for (idx = 0; idx < m_Outputs.size(); idx++)
    {
    if (m_Outputs[idx])
      {
      m_Outputs[idx]->SetReleaseDataFlag(val);
      }
    }
}


/**
 *
 */
void 
ProcessObject
::PrintSelf(std::ostream& os, Indent indent)
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Number Of Required Inputs: "
     << m_NumberOfRequiredInputs << std::endl;

  if ( m_Inputs.size())
    {
    std::vector<DataObjectPointer>::size_type idx;
    for (idx = 0; idx < m_Inputs.size(); ++idx)
      {
      os << indent << "Input " << idx << ": (" << m_Inputs[idx] << ")\n";
      }
    }
  else
    {
    os << indent <<"No Inputs\n";
    }
  if ( m_Outputs.size())
    {
    std::vector<DataObjectPointer>::size_type idx;
    for (idx = 0; idx < m_Outputs.size(); ++idx)
      {
      os << indent << "Output " << idx << ": (" << m_Outputs[idx] << ")\n";
      }
    }
  else
    {
    os << indent <<"No Output\n";
    }

  os << indent << "AbortGenerateData: " << (m_AbortGenerateData ? "On\n" : "Off\n");
  os << indent << "Progress: " << m_Progress << "\n";

  os << indent << "Multithreader: " << m_Threader << std::endl;
}



/**
 * The following methods are used to coordinate the execution of the
 * data processing pipeline.
 */


/**
 *
 */
void 
ProcessObject
::Update()
{
  if (this->GetOutput(0))
    {
    this->GetOutput(0)->Update();
    }
}


/**
 *
 */
void 
ProcessObject
::UpdateOutputInformation()
{
  unsigned long t1, t2;
  std::vector<DataObjectPointer>::size_type idx;
  DataObject *input;
  DataObject *output;

  /**
   * Watch out for loops in the pipeline
   */
  if ( m_Updating )
    {
    /**
     * Since we are in a loop, we will want to update. But if
     * we don't modify this filter, then we will not execute
     * because our InformationTime will be more recent than
     * the MTime of our output.
     */
    this->Modified();
    return;
    }

  /**
   * The MTime of this source will be used in determine the PipelineMTime
   * for the outputs
   */
  t1 = this->GetMTime();

  /**
   * Loop through the inputs
   */
  for (idx = 0; idx < m_Inputs.size(); ++idx)
    {
    if (m_Inputs[idx])
      {
      input = m_Inputs[idx];

      /**
       * Propagate the UpdateOutputInformation call
       */
      m_Updating = true;
      input->UpdateOutputInformation();
      m_Updating = false;
      
      /**
       * What is the PipelineMTime of this input? Compare this against
       * our current computation to find the largest one.
       */
      t2 = input->GetPipelineMTime();

      if (t2 > t1)
        {
        t1 = t2;
        }

      /**
       * Pipeline MTime of the input does not include the MTime of the 
       * data object itself. Factor these mtimes into the next PipelineMTime
       */
      t2 = input->GetMTime();
      if (t2 > t1)
        {
        t1 = t2;
        }
      }
    }

  /**
   * Call GenerateOutputInformation for subclass specific information.
   * Since UpdateOutputInformation propagates all the way up the pipeline,
   * we need to be careful here to call GenerateOutputInformation only if
   * necessary. Otherwise, we may cause this source to be modified which
   * will cause it to execute again on the next update.
   */
  if (t1 > m_InformationTime.GetMTime())
    {
    for (idx = 0; idx < m_Outputs.size(); ++idx)
      {
      output = this->GetOutput(idx);
      if (output)
        {
        output->SetPipelineMTime(t1);
        }  
      }
    
    this->GenerateOutputInformation();
    }
}


/**
 *
 */
void 
ProcessObject
::PropagateRequestedRegion(DataObject *output)
{
  /**
   * check flag to avoid executing forever if there is a loop
   */
  if (m_Updating)
    {
    return;
    }

  /**
   * Give the subclass a chance to indicate that it will provide
   * more data then require for the output. This can happen, for
   * example, when a source can only produce the whole output.
   * Although this is being called for a specific output, the source
   * may need to enlarge all outputs.
   */
  this->EnlargeOutputRequestedRegion( output );

  /**
   * Give the subclass a chance to define how to set the requested
   * regions for each of its outputs, given this output's requested
   * region.  The default implementation is to make all the output
   * requested regions the same.  A subclass may need to override this
   * method if each output is a different resolution.
   */
  this->GenerateOutputRequestedRegion( output );
  
  /**
   * Give the subclass a chance to request a larger requested region on 
   * the inputs. This is necessary when, for example, a filter
   * requires more data at the "internal" boundaries to 
   * produce the boundary values - such as an image filter that
   * derives a new pixel value by applying some operation to a 
   * neighborhood of surrounding original values. 
   */
  this->GenerateInputRequestedRegion();

  /**
   * Now that we know the input requested region, propagate this
   * through all the inputs.
   */
  m_Updating = true;
  std::vector<DataObjectPointer>::size_type idx;
  for (idx = 0; idx < m_Inputs.size(); ++idx)
    {
    if (m_Inputs[idx])
      {
      m_Inputs[idx]->PropagateRequestedRegion();
      }
    }
  m_Updating = false;
}


/**
 * By default we require all the input to produce the output. This is
 * overridden in the subclasses since we can often produce the output with
 * just a portion of the input data.
 */
void 
ProcessObject
::GenerateInputRequestedRegion()
{
  std::vector<DataObjectPointer>::size_type idx;
  for (idx = 0; idx < m_Inputs.size(); ++idx)
    {
    if (m_Inputs[idx])
      {
      m_Inputs[idx]->SetRequestedRegionToLargestPossibleRegion();
      }
    }  
}


/**
 * By default we set all the output requested regions to be the same.
 */
void 
ProcessObject
::GenerateOutputRequestedRegion(DataObject *output)
{
  std::vector<DataObjectPointer>::size_type idx;
  for (idx = 0; idx < m_Outputs.size(); ++idx)
    {
    if (m_Outputs[idx] && m_Outputs[idx] != output)
      {
      m_Outputs[idx]->SetRequestedRegion(output);
      }
    }  
}


/**
 *
 */
void 
ProcessObject
::UpdateOutputData(DataObject *itkNotUsed(output))
{
  std::vector<DataObjectPointer>::size_type idx;

  /**
   * prevent chasing our tail
   */
  if (m_Updating)
    {
    return;
    }

  /**
   * Propagate the update call - make sure everything we
   * might rely on is up-to-date
   * Must call PropagateRequestedRegion before UpdateOutputData if multiple 
   * inputs since they may lead back to the same data object.
   */
  m_Updating = true;
  if ( m_Inputs.size() == 1 )
    {
    if (m_Inputs[0])
      {
      m_Inputs[0]->UpdateOutputData();
      }
    }
  else
    {
    for (idx = 0; idx < m_Inputs.size(); ++idx)
      {
      if (m_Inputs[idx])
        {
        m_Inputs[idx]->PropagateRequestedRegion();
        m_Inputs[idx]->UpdateOutputData();
        }
      }
    }
  m_Updating = false;     
    
  /**
   * Initialize all the outputs
   */
  for (idx = 0; idx < m_Outputs.size(); idx++)
    {
    if (m_Outputs[idx])
      {
      m_Outputs[idx]->PrepareForNewData(); 
      }
    }
 
  this->InvokeEvent(Command::StartEvent);

  /**
   * GenerateData this object - we have not aborted yet, and our progress
   * before we start to execute is 0.0.
   */
  m_AbortGenerateData = 0;
  m_Progress = 0.0;
  if (m_Inputs.size() < m_NumberOfRequiredInputs)
    {
    itkErrorMacro(<< "At least " << m_NumberOfRequiredInputs << " inputs are required but only " << m_Inputs.size() << " are specified");
    }
  else
    {
    this->GenerateData();
    }

  /**
   * If we ended due to aborting, push the progress up to 1.0 (since
   * it probably didn't end there)
   */
  if ( !m_AbortGenerateData )
    {
    this->UpdateProgress(1.0);
    }

  // Notify end event observers
  this->InvokeEvent(Command::EndEvent);

  /**
   * Now we have to mark the data as up to data.
   */
  for (idx = 0; idx < m_Outputs.size(); ++idx)
    {
    if (m_Outputs[idx])
      {
      m_Outputs[idx]->DataHasBeenGenerated();
      }
    }
  
  /**
   * Release any inputs if marked for release
   */
  for (idx = 0; idx < m_Inputs.size(); ++idx)
    {
    if (m_Inputs[idx])
      {
      if ( m_Inputs[idx]->ShouldIReleaseData() )
        {
        m_Inputs[idx]->ReleaseData();
        }
      }  
    }
  
  /**
   * Information gets invalidated as soon as Update is called,
   * so validate it again here.
   */
  m_InformationTime.Modified();
}


/**
 *
 */
void 
ProcessObject
::ComputeEstimatedPipelineMemorySize(DataObject *output,
                                     unsigned long size[3] )
{
  unsigned long outputSize[2];
  unsigned long inputPipelineSize[3];
  unsigned long mySize = 0;
  unsigned long maxSize = 0;
  unsigned long goingDownstreamSize = 0;
  unsigned long *inputSize = NULL;
  std::vector<DataObjectPointer>::size_type idx;

  /**
   * We need some space to store the input sizes if there are any inputs
   */
  if ( m_Inputs.size() > 0 )
    {
    inputSize = new unsigned long[m_Inputs.size()];
    }

  /**
   * Get the pipeline size propagated down each input. Keep track of max
   * pipeline size, how much memory will be required downstream from here,
   * the size of each input, and the memory required by this filter when
   * it executes.
   */
  for (idx = 0; idx < m_Inputs.size(); ++idx)
    {
    if (m_Inputs[idx])
      {

      /**
       * Get the upstream size of the pipeline, the estimated size of this
       * input, and the maximum size seen upstream from here.
       */
      m_Inputs[idx]->ComputeEstimatedPipelineMemorySize(inputPipelineSize);

      /**
       * Save this input size to possibly be used when estimating output size
       */
      inputSize[idx] = inputPipelineSize[1];

      /**
       * Is the max returned bigger than the max we've seen so far?
       */
      if ( inputPipelineSize[2] > maxSize )
        {
        maxSize = inputPipelineSize[2];
        }

      /**
       * If we are going to release this input, then its size won't matter
       * downstream from here.
       */
      if ( m_Inputs[idx]->ShouldIReleaseData() )
        {
        goingDownstreamSize += inputPipelineSize[0] - inputPipelineSize[1];
        }
      else
        {
        goingDownstreamSize += inputPipelineSize[0];
        }

      /**
       * During execution this filter will need all the input data 
       */
      mySize += inputPipelineSize[0];
      }

    /**
     * The input was null, so it has no size
     */
    else
      {
      inputSize[idx] = 0;
      }
    }

  /**
   * Now the we know the size of all input, compute the output size
   */
  this->ComputeEstimatedOutputMemorySize( output, inputSize, outputSize );

  /**
   * This filter will produce all output so it needs all that memory.
   * Also, all this data will flow downstream to the next source (if it is
   * the requested output) or will still exist with no chance of being
   * released (if it is the non-requested output)
   */
  mySize += outputSize[1];
  goingDownstreamSize += outputSize[1];

  /**
   * Is the state of the pipeline during this filter's execution the
   * largest that it has been so far?
   */
  if ( mySize > maxSize )
    {
    maxSize = mySize;
    }

  /**
   * The first size is the memory going downstream from here - which is all
   * the memory coming in minus any data realeased. The second size is the
   * size of the specified output (which can be used by the downstream 
   * filter when determining how much data it might release). The final size
   * is the maximum pipeline size encountered here and upstream from here.
   */
  size[0] = goingDownstreamSize;
  size[1] = outputSize[0];
  size[2] = maxSize;

  /**
   * Delete the space we may have created
   */
  if ( inputSize )
    {
    delete [] inputSize;
    }
}


/**
 * This default implementation can be used by any source that will produce
 * only image output. This method should be overridden by anything
 * that will produce mesh (unstructured) output since the
 * output itself cannot estimate its own size.
 */
void 
ProcessObject
::ComputeEstimatedOutputMemorySize( DataObject *output,
                                          unsigned long *itkNotUsed(inputSize),
                                          unsigned long size[2] )
{
  unsigned int idx;
  unsigned int tmp;

  size[0] = 0;
  size[1] = 0;

  /**
   * loop through all the outputs asking them how big they are given the
   * information that they have on their requested region. Keep track of 
   * the size of the specified output in size[0], and the sum of all
   * output size in size[1]. Ignore input sizes in this default implementation.
   */
  for (idx = 0; idx < m_Outputs.size(); ++idx)
    {
    if (m_Outputs[idx])
      {
      tmp = m_Outputs[idx]->GetEstimatedMemorySize();
      if ( m_Outputs[idx] == output )
        {
        size[0] = tmp;
        }
      size[1] += tmp;
      }
    }
}


/**
 * Default implementation - copy information from first input to all outputs
 */
void 
ProcessObject
::GenerateOutputInformation()
{
  DataObjectPointer input, output;

  if (m_Inputs.size() && m_Inputs[0])
    {
    input = m_Inputs[0];

    for (unsigned int idx = 0; idx < m_Outputs.size(); ++idx)
      {
      output = this->GetOutput(idx);
      if (output)
        {
        output->CopyInformation(input);
        }  
      }
    }
}


/**
 *
 */
void 
ProcessObject
::UpdateLargestPossibleRegion()
{
  this->UpdateOutputInformation();

  if (this->GetOutput(0))
    {
    this->GetOutput(0)->SetRequestedRegionToLargestPossibleRegion();
    this->GetOutput(0)->Update();
    }
}




} // end namespace itk
