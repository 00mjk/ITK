/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkNormalizedCorrelationImageToImageMetric.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNormalizedCorrelationImageToImageMetric_h
#define __itkNormalizedCorrelationImageToImageMetric_h

#include "itkSimilarityRegistrationMetric.h"
#include "itkCovariantVector.h"
#include "itkPoint.h"


namespace itk
{
  
/** \class NormalizedCorrelationImageToImageMetric
 * \brief Computes similarity between two objects to be registered
 *
 * This Class is templated over the type of the objects to be registered and
 * over the type of transformation to be used.
 *
 * SmartPointer to this three objects are received, and using them, this
 * class computes a value(s) that measures the similarity of the target
 * against the reference object once the transformation is applied to it.
 *
 *
 * \ingroup RegistrationMetrics
 *
 */
template < class TTarget, class TMapper > 
class ITK_EXPORT NormalizedCorrelationImageToImageMetric : 
public SimilarityRegistrationMetric< TTarget, TMapper, double,
               CovariantVector<double, TMapper::SpaceDimension > >
{
public:
  /** Space dimension is the dimension of parameters space. */
  enum { SpaceDimension = TMapper::SpaceDimension };
  enum { RangeDimension = 9};

  /** Standard class typedefs. */
  typedef double              MeasureType;
  typedef CovariantVector<MeasureType,SpaceDimension >  DerivativeType;
  typedef NormalizedCorrelationImageToImageMetric  Self;
  typedef SimilarityRegistrationMetric< 
                       TTarget, TMapper,
                       MeasureType,DerivativeType >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
 
  /** Run-time type information (and related methods). */
  itkTypeMacro(NormalizedCorrelationImageToImageMetric, Object);

  /**  Type of the mapper. */
  typedef TMapper             MapperType;
  
  /**  Type of the reference. */
  typedef typename MapperType::DomainType     ReferenceType;

  /**  Type of the target. */
  typedef TTarget             TargetType;

  /**  Pointer type for the reference.  */
  typedef typename ReferenceType::ConstPointer         ReferenceConstPointer;

  /**  Pointer type for the target.  */
  typedef typename TargetType::ConstPointer            TargetConstPointer;

  /**  Pointer type for the mapper. */
  typedef typename MapperType::Pointer            MapperPointer;

  /**  Parameters type. */
  typedef typename  TMapper::ParametersType       ParametersType;

  /** Get the derivatives of the match measure. */
  const DerivativeType & GetDerivative( const ParametersType & parameters );

  /**  Get the value for single valued optimizers. */
  MeasureType    GetValue( const ParametersType & parameters );

  /**  Get the value and derivatives for multiple valued optimizers. */
   void GetValueAndDerivative( const ParametersType & parameters,
                               MeasureType& Value, DerivativeType& Derivative);
protected:
  NormalizedCorrelationImageToImageMetric();
  virtual ~NormalizedCorrelationImageToImageMetric() {};

private:
  NormalizedCorrelationImageToImageMetric(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNormalizedCorrelationImageToImageMetric.txx"
#endif

#endif



