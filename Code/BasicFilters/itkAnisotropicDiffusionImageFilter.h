/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkAnisotropicDiffusionImageFilter.h
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
#ifndef __itkAnisotropicDiffusionImageFilter_h_
#define __itkAnisotropicDiffusionImageFilter_h_

#include "itkDenseFiniteDifferenceImageFilter.h"
#include "itkAnisotropicDiffusionFunction.h"
#include "itkNumericTraits.h"

namespace itk {

/** \class AnisotropicDiffusionImageFilter
 * \ingroup ImageEnhancement
 */
template <class TInputImage, class TOutputImage>
class AnisotropicDiffusionImageFilter
  : public DenseFiniteDifferenceImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef AnisotropicDiffusionImageFilter Self;
  typedef DenseFiniteDifferenceImageFilter<TInputImage, TOutputImage>
   Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Smart pointer support for this class. */
  itkNewMacro(Self);

  /** Run-time type information. */
  itkTypeMacro(AnisotropicDiffusionImageFilter,
               DenseFiniteDifferenceImageFilter);

  /** Capture information from the superclass. */
  typedef typename Superclass::InputImageType   InputImageType;
  typedef typename Superclass::OutputImageType  OutputImageType;
  typedef typename Superclass::UpdateBufferType UpdateBufferType;

  /** Dimensionality of input and output data is assumed to be the same.
   * It is inherited from the superclass. */
  enum { ImageDimension = Superclass::ImageDimension };

  /** The pixel type of the output image will be used in computations.
   * Inherited from the superclass. */
  typedef typename Superclass::PixelType PixelType;
  typedef typename Superclass::TimeStepType TimeStepType;

  /** Set/Get the number of iterations that the filter will run. */
  itkSetMacro(Iterations, unsigned int);
  itkGetMacro(Iterations, unsigned int);

  /** Set/Get the time step for each iteration */
  itkSetMacro(TimeStep, TimeStepType);
  itkGetMacro(TimeStep, TimeStepType);

  /** Set/Get the conductance parameter governing sensitivity of the
      conductance equation. */
  itkSetMacro(ConductanceParameter, double);
  itkGetMacro(ConductanceParameter, double);
  itkSetMacro(ConductanceScalingUpdateInterval, unsigned int);
  itkGetMacro(ConductanceScalingUpdateInterval, unsigned int);
  itkSetMacro(ConductanceScalingParameter, double);
  itkGetMacro(ConductanceScalingParameter, double);

  void SetFixedAverageGradientMagnitude(double a)
    {
      m_FixedAverageGradientMagnitude= a;
      this->Modified();
      m_GradientMagnitudeIsFixed = true;
    }
  itkGetMacro(FixedAverageGradientMagnitude, double);
  
protected:
  AnisotropicDiffusionImageFilter()
    {
      m_Iterations = 0;
      m_ConductanceParameter = 1.0;
      m_TimeStep = 0.125f;
      m_FixedAverageGradientMagnitude = 0.0;
      m_GradientMagnitudeIsFixed = false;
    }
  ~AnisotropicDiffusionImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const
    {
      Superclass::PrintSelf(os, indent.GetNextIndent());
    }
  
  /** Supplies the halting criteria for this class of filters.  The
   * algorithm will stop after a user-specified number of iterations. */
  virtual bool Halt()
    {
      if (this->GetElapsedIterations() == m_Iterations) return true;
      else return false;
    }

  /** Prepare for the iteration process. */
  virtual void InitializeIteration()
    {
      AnisotropicDiffusionFunction<UpdateBufferType> *f = 
        dynamic_cast<AnisotropicDiffusionFunction<UpdateBufferType> *>
        (this->GetDifferenceFunction().GetPointer());
      f->SetConductanceParameter(m_ConductanceParameter);
      
      if (m_GradientMagnitudeIsFixed == false)
        {
          f->CalculateAverageGradientMagnitudeSquared(this->GetOutput());
        }
      else
        {
          f->SetAverageGradientMagnitudeSquared(m_FixedAverageGradientMagnitude 
                                                *
                                                m_FixedAverageGradientMagnitude);
        }
      f->InitializeIteration();

      if (m_Iterations != 0)
          this->UpdateProgress(((float)(this->GetElapsedIterations()))
                               /((float)(m_Iterations)));
      else this->UpdateProgress(0);
    }

  bool m_GradientMagnitudeIsFixed;
  
private:
  AnisotropicDiffusionImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  double           m_ConductanceParameter;
  double           m_ConductanceScalingParameter;
  unsigned int     m_Iterations;
  unsigned int     m_ConductanceScalingUpdateInterval;
  double           m_FixedAverageGradientMagnitude;
  

  TimeStepType     m_TimeStep;
};

} // end namspace itk

#endif
