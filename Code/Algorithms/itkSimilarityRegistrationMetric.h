/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkSimilarityRegistrationMetric.h
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
#ifndef __itkSimilarityRegistrationMetric_h
#define __itkSimilarityRegistrationMetric_h

#include "itkObject.h"

namespace itk
{
  
/** \class SimilarityRegistrationMetric
 * \brief Computes similarity between two objects to be registered
 *
 * This Class is templated over the type of the objects to be registered and
 * over the type of transformation to be used.
 *
 * SmartPointer to this three objects are received, and using them, this
 * class computes a value(s) that measures the similarity of the target
 * against the reference object once the transformation is applied to it.
 *
 * The class is templated over the kind of value that can be produced as
 * measure of similarity. That allows to cover methods that produce residuals
 * as well as methods that produces just one double as result.
 *
 * \ingroup RegistrationMetrics
 *
 */

template <class TTarget,  class TMapper, 
          class TMeasure, class TDerivative > 
class ITK_EXPORT SimilarityRegistrationMetric : public Object 

{
public:
  /** Standard class typedefs. */
  typedef SimilarityRegistrationMetric  Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SimilarityRegistrationMetric, Object);

  /**  Type of the reference. */
  typedef typename TMapper::DomainType          ReferenceType;
  typedef typename ReferenceType::ConstPointer  ReferenceConstPointer;

  /**  Type of the target. */
  typedef TTarget               TargetType;

  /**  Type of the mapper. */
  typedef TMapper               MapperType;

  /**  Type of the measure. */
  typedef TMeasure              MeasureType;

  /**  Type of the measure. */
  typedef TDerivative           DerivativeType;

  /**  Pointer type for the target.  */
  typedef typename TargetType::ConstPointer TargetConstPointer;

  /**  Pointer type for the mapper. */
  typedef typename MapperType::Pointer MapperPointer;

  /** Method for execute the algorithm. */
  virtual void Compute(void);

  /** Connect the reference.  */
  void SetReference( const ReferenceType * );

  /** Return the reference.  */
  ReferenceConstPointer  GetReference( void );

  /** Connect the target.  */
  itkSetConstObjectMacro( Target, TargetType );

  /** Get the target. */
  itkGetConstObjectMacro( Target, TargetType );

  /** Get the match measure value. */
  itkGetMacro( MatchMeasure, MeasureType );

  /** Get the derivatives of the match measure. */
  itkGetMacro( MatchMeasureDerivatives, DerivativeType );
  
  /** Connect the mapper. */
  itkSetObjectMacro( Mapper, MapperType );

  /** Get a pointer to the mapper.  */
  itkGetObjectMacro( Mapper, MapperType );

protected:
  SimilarityRegistrationMetric();
  virtual ~SimilarityRegistrationMetric() {};
  void PrintSelf(std::ostream& os, Indent indent) const;


  MeasureType                 m_MatchMeasure;
  DerivativeType              m_MatchMeasureDerivatives;

private:
  SimilarityRegistrationMetric(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  TargetConstPointer          m_Target;
  MapperPointer               m_Mapper;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSimilarityRegistrationMetric.txx"
#endif

#endif



