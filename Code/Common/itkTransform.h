/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkTransform.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTransform_h
#define __itkTransform_h

#include "itkObject.h"
#include "itkPoint.h"
#include "itkVector.h"
#include "itkCovariantVector.h"
#include "vnl/vnl_vector_fixed.h"
#include "itkArray.h"
#include "itkArray2D.h"

#include "itkObjectFactory.h"


namespace itk
{
  
/** \class Transform
 * \brief Generic concept of transformation methods
 *
 * This Abstract Class define the generic interface for a geometrical 
 * transformation from one space to another. The class provides methods
 * for mapping points from the input space to the output space, it also
 * maps vectors and covariant vectors. 
 *
 * Given that transformation are not necesarily invertible, this basic
 * class provide the methods for back transfromation. Derived classes that
 * implement non-invertible transformation should throw exceptions when
 * appropriate.
 *
 * \ingroup Transforms
 *
 */
template <class TScalarType,
          unsigned int NInputDimensions=3, 
          unsigned int NOutputDimensions=3>
class ITK_EXPORT  Transform  : public Object
{
public:
  /** Standard class typedefs. */
  typedef Transform  Self;
  typedef Object Superclass;
  typedef SmartPointer< Self >   Pointer;
  typedef SmartPointer< const Self >  ConstPointer;
  
  /** New method for creating an object using a factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( Transform, Object );

  /** Dimension of the domain space. */
  enum { InputSpaceDimension     = NInputDimensions,
         OutputSpaceDimension    = NOutputDimensions };

  /** Type of the input parameters. */
  typedef  TScalarType     ScalarType;

  /** Type of the input parameters. */
  typedef  Array< double >                           ParametersType;

  /** Type of the Jacobian matrix. */
  typedef  Array2D< double >                           JacobianType;

  /** Standard vector type for this class. */
  typedef Vector<TScalarType, InputSpaceDimension>  InputVectorType;
  typedef Vector<TScalarType, OutputSpaceDimension> OutputVectorType;
  
  /** Standard covariant vector type for this class */
  typedef CovariantVector<TScalarType, InputSpaceDimension>  InputCovariantVectorType;
  typedef CovariantVector<TScalarType, OutputSpaceDimension> OutputCovariantVectorType;
  
  /** Standard vnl_vector type for this class. */
  typedef vnl_vector_fixed<TScalarType, InputSpaceDimension>  InputVnlVectorType;
  typedef vnl_vector_fixed<TScalarType, OutputSpaceDimension> OutputVnlVectorType;
  
  /** Standard coordinate point type for this class */
  typedef Point<TScalarType, InputSpaceDimension> InputPointType;
  typedef Point<TScalarType, OutputSpaceDimension> OutputPointType;
  
  /**  Method to transform a point. */
  virtual OutputPointType TransformPoint(const InputPointType  &point ) const
    { return OutputPointType(); } 

  /**  Method to transform a vector. */
  virtual OutputVectorType    TransformVector(const InputVectorType &vector) const
    { return OutputVectorType(); }

  /**  Method to transform a vnl_vector. */
  virtual OutputVnlVectorType TransformVector(const InputVnlVectorType &vector) const
    { return OutputVnlVectorType(); }

  /**  Method to transform a CovariantVector. */
  virtual OutputCovariantVectorType TransformCovariantVector(
    const InputCovariantVectorType &vector) const
    { return OutputCovariantVectorType(); } 

  /** Set the Transformation Parameters
   * and update the internal transformation. */
  virtual void SetParameters(const ParametersType &) {};

  /** Get the Transformation Parameters. */
  virtual const ParametersType& GetParameters(void) const
    { return m_Parameters; }

  /** Compute the Jacobian of the transformation
   *
   * This method computes the Jacobian matrix of the transformation.
   * given point or vector, returning the transformed point or
   * vector. The rank of the Jacobian will also indicate if the transform
   * is invertible at this point.
   *
   * The Jacobian can be expressed as a set of partial derivatives of the
   * output point components with respect to the parameters that defined
   * the transform:
   *
   * \f[
   *
      J=\left[ \begin{array}{cccc}
      \frac{\partial x_{1}}{\partial p_{1}} & 
      \frac{\partial x_{2}}{\partial p_{1}} & 
      \cdots  & \frac{\partial x_{n}}{\partial p_{1}}\\
      \frac{\partial x_{1}}{\partial p_{2}} & 
      \frac{\partial x_{2}}{\partial p_{2}} & 
      \cdots  & \frac{\partial x_{n}}{\partial p_{2}}\\
      \vdots  & \vdots  & \ddots  & \vdots \\
      \frac{\partial x_{1}}{\partial p_{m}} & 
      \frac{\partial x_{2}}{\partial p_{m}} & 
      \cdots  & \frac{\partial x_{n}}{\partial p_{m}}
      \end{array}\right] 
   *
   * \f]
   * **/
  virtual const JacobianType & GetJacobian(const InputPointType  &point ) const
    { 
    std::cout << "This message should never show up" << std::endl;
    m_Jacobian = JacobianType(NInputDimensions,1); 
    m_Jacobian.Fill(0.0); 
    return m_Jacobian;
    }


  /** Return the number of parameters that completely define the Transfom  */
  unsigned int virtual GetNumberOfParameters(void) const 
                      { return m_Parameters.Size(); }


protected:
  Transform(); 
  Transform(unsigned int Dimension, unsigned int NumberOfParameters);
  virtual ~Transform() {};


  mutable ParametersType     m_Parameters;

  mutable JacobianType       m_Jacobian;

private:
  Transform(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTransform.txx"
#endif

#endif



