/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkScaleTransform.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkScaleTransform_h
#define __itkScaleTransform_h

#include <iostream>
#include "itkTransform.h"
#include "itkExceptionObject.h"
#include "itkMatrix.h"

namespace itk
{

/** \brief Scale transformation of a vector space (e.g. space coordinates)
 *
 * The same functionality could be obtained by using the Affine tranform,
 * but with a large difference in performace
 *
 * \ingroup Transforms
 */
template <
    class TScalarType=double,          // Data type for scalars (float or double)
    unsigned int NDimensions=3,
    class TParameters = Point< double, NDimensions >,
    class TJacobianType = Matrix<double,NDimensions,NDimensions > 
    >  // Number of dimensions
class ScaleTransform : public Transform< TScalarType, 
                                         NDimensions,
                                         NDimensions,
                                         TParameters,
                                         TJacobianType >
{
public:
  /** Standard class typedefs.   */
  typedef ScaleTransform Self;
  typedef Transform< TScalarType, NDimensions,
                     NDimensions, TParameters, 
                     TJacobianType >             Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** New macro for creation of through a smart pointer. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( ScaleTransform, Transform );

  /** Dimension of the domain space. */
  enum { SpaceDimension      = NDimensions,
         ParametersDimension = NDimensions };

  /** Scalar type. */
  typedef typename Superclass::ScalarType  ScalarType;

  /** Parameters type. */
  typedef typename Superclass::ParametersType  ParametersType;

  /** Jacobian type. */
  typedef typename Superclass::JacobianType  JacobianType;

  /** Standard vector type for this class. */
  typedef FixedArray<TScalarType, SpaceDimension> ScaleType;

  /** Standard vector type for this class. */
  typedef Vector<TScalarType, SpaceDimension> InputVectorType;
  typedef Vector<TScalarType, SpaceDimension> OutputVectorType;
  
  /** Standard covariant vector type for this class. */
  typedef CovariantVector<TScalarType, SpaceDimension> InputCovariantVectorType;
  typedef CovariantVector<TScalarType, SpaceDimension> OutputCovariantVectorType;
  
  /** Standard vnl_vector type for this class. */
  typedef vnl_vector_fixed<TScalarType, SpaceDimension> InputVnlVectorType;
  typedef vnl_vector_fixed<TScalarType, SpaceDimension> OutputVnlVectorType;
  
  /** Standard coordinate point type for this class. */
  typedef Point<TScalarType, SpaceDimension> InputPointType;
  typedef Point<TScalarType, SpaceDimension> OutputPointType;
  
  /** Get scale of an ScaleTransform.
   * This method returns the value of the offset of the
   * ScaleTransform. */
  const ScaleType & GetScale( void ) const
    { return m_Scale; }

  /** Set parameters.
   * This method sets the parameters for the transform
   * value specified by the user. */
  void SetParameters(const ParametersType & parameters);

  /** Get the Jacobian matrix. */
  const JacobianType & GetJacobian( const InputPointType & point ) const;

  /** Set offset of an Scale Transform
   * This method sets the offset of an ScaleTransform to a
   * value specified by the user. 
   * This method cannot be done with SetMacro because itk::Array has not
   * an operator== defined. */
  void SetScale( const ScaleType & scale )
    { this->Modified(); m_Scale = scale; }

  /** Compose with another ScaleTransform. */
  void Compose(const Self * other, bool pre=false);

  /** Compose affine transformation with a translation
   * This method modifies self to include a translation of the
   * origin.  The translation is precomposed with self if pre is
   * true, and postcomposed otherwise. */
  void Scale(const ScaleType & scale, bool pre=false );

  /** Transform by an affine transformation
   * This method applies the affine transform given by self to a
   * given point or vector, returning the transformed point or
   * vector. */
  OutputPointType     TransformPoint(const InputPointType  &point ) const;
  OutputVectorType    TransformVector(const InputVectorType &vector) const;
  OutputVnlVectorType TransformVector(const InputVnlVectorType &vector) const;
  OutputCovariantVectorType TransformCovariantVector(
                                 const InputCovariantVectorType &vector) const;
  
  /** Back transform by an affine transformation
   * This method finds the point or vector that maps to a given
   * point or vector under the affine transformation defined by
   * self.  If no such point exists, an exception is thrown. */
  inline InputPointType     BackTransform(const OutputPointType  &point ) const;
  inline InputVectorType    BackTransform(const OutputVectorType &vector) const;
  inline InputVnlVectorType BackTransform(const OutputVnlVectorType &vector) const;
  inline InputCovariantVectorType BackTransform(
                                     const OutputCovariantVectorType &vector) const;
    
  /** Find inverse of an affine transformation
   * This method creates and returns a new ScaleTransform object
   * which is the inverse of self.  If self is not invertible,
   * an exception is thrown. */
  ScaleTransform::Pointer Inverse(void) const;

protected:
  /** Construct an ScaleTransform object. */
  ScaleTransform();

  /** Destroy an ScaleTransform object. */
  ~ScaleTransform();

  /** Print contents of an ScaleTransform */
  void PrintSelf(std::ostream &os, Indent indent) const;

private:
  ScaleTransform(const Self & other); //purposely not implemented
  const Self & operator=( const Self & ); //purposely not implemented

  ScaleType   m_Scale;  // Scales of the transformation

}; //class ScaleTransform

}  // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkScaleTransform.txx"
#endif

#endif /* __itkScaleTransform_h */
