/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkProgressReporter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkProgressReporter_h
#define _itkProgressReporter_h

#include "itkProcessObject.h"

namespace itk
{

/** \class ProgressReporter
 * \brief Implements progress tracking for a filter.
 *
 * This is a utility class for use by filter implementations in
 * GenerateData() and ThreadedGenerateData().
 *
 * The class constructor sets the progress to 0, and the destructor
 * sets it to 1.  In between, there should be one call to
 * CompletedPixel() per pixel.  The reporter will automatically update
 * the filter's progress at an interval resulting in the specified
 * number of updates.  The default number of updates is 100.
 *
 * Example usage:
 *
 *   ProgressReporter progress(this, threadId,
 *                             threadRegion.GetNumberOfPixels(), 100);
 *   for( each pixel )
 *     {
 *     ...
 *     progress.CompletedPixel();
 *     }
 *
 * When used in a non-threaded filter, the threadId argument should be 0.
 */
class ProgressReporter
{
public:
  /** Constructor sets progress to 0 because the filter is starting.  */
  ProgressReporter(ProcessObject* filter, int threadId,
                   unsigned long numberOfPixels,
                   unsigned long numberOfUpdates = 100);
  
  /** Destructor sets progress to 1 because the filter has finished.  */
  ~ProgressReporter();
  
  /** Called by a filter once per pixel.  */
  void CompletedPixel()
    {
    // Inline implementation for efficiency.  We don't want to add a
    // method call per pixel.
    if((m_ThreadId == 0) && (--m_PixelsBeforeUpdate == 0))
      {
      m_PixelsBeforeUpdate = m_PixelsPerUpdate;
      m_CurrentPixel += m_PixelsPerUpdate;
      m_Filter->UpdateProgress(m_CurrentPixel * m_InverseNumberOfPixels);
      }
    }
protected:
  ProcessObject* m_Filter;
  int m_ThreadId;
  float m_InverseNumberOfPixels;
  unsigned long m_NumberOfUpdates;
  unsigned long m_CurrentPixel;
  unsigned long m_PixelsPerUpdate;
  unsigned long m_PixelsBeforeUpdate;
};

} // end namespace itk

#endif
