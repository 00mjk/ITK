/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkRigid3DPerspectiveTransform.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkRigid3DPerspectiveTransform_h
#define __itkRigid3DPerspectiveTransform_h

#include "itkExceptionObject.h"
#include "vnl/vnl_quaternion.h"
#include <iostream>
#include "itkMatrix.h"
#include "itkTransform.h"
#include "itkVersor.h"

namespace itk
{

/** \brief Rigid3DTramsform of a vector space (e.g. space coordinates)
 *
 * This transform applies a rotation and translation to the 3D space
 * followed by a projection to 2D space along the Z axis.
 *
 * \ingroup Transforms
 */

template <
    class TScalarType=double>    // Data type for scalars (float or double)
class ITK_EXPORT Rigid3DPerspectiveTransform : 
        public Transform<  TScalarType, 3,
                           2, Point<TScalarType,6>, 
                           Matrix<TScalarType,2,6>   > 
{
public:
  /** Dimension of the domain space. */
  enum 
  { 
    InputSpaceDimension      = 3,
    OutputSpaceDimension     = 2
  };

  /** Dimension of parameters. */
  enum { SpaceDimension = 3, 
         ParametersDimension = 6 };

  /** Standard class typedefs. */ 
  typedef Rigid3DPerspectiveTransform Self;
  typedef Transform< TScalarType, 
    InputSpaceDimension,
    OutputSpaceDimension, 
    Point<TScalarType,ParametersDimension>, 
    Matrix<TScalarType,OutputSpaceDimension,ParametersDimension> > Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro( Rigid3DPerspectiveTransform, Transform );

  /** New macro for creation of through a Smart Pointer. */
  itkNewMacro( Self );

  /** Scalar type. */
  typedef typename Superclass::ScalarType  ScalarType;

  /** Parameters type. */
  typedef typename Superclass::ParametersType  ParametersType;

  /** Jacobian type. */
  typedef typename Superclass::JacobianType  JacobianType;

  /** Standard matrix type for this class. */
  typedef Matrix<TScalarType, InputSpaceDimension, InputSpaceDimension> MatrixType;

  /** Standard vector type for this class. */
  typedef Vector<TScalarType, InputSpaceDimension> OffsetType;

  /** Standard vector type for this class. */
  typedef Vector<TScalarType, InputSpaceDimension> InputVectorType;
  typedef Vector<TScalarType, OutputSpaceDimension> OutputVectorType;
  
  /** Standard coordinate point type for this class. */
  typedef Point<TScalarType, InputSpaceDimension>    InputPointType;
  typedef Point<TScalarType, OutputSpaceDimension>    OutputPointType;
  
  /** Standard vnl_quaternion type. */
  typedef vnl_quaternion<TScalarType>           VnlQuaternionType;

  /** Versor type. */
  typedef Versor<TScalarType>             VersorType;
  typedef typename VersorType::VectorType  AxisType;
  typedef typename VersorType::ValueType   AngleType;
  
  /** Get offset of an Rigid3DPerspectiveTransform
   * This method returns the value of the offset of the
   * Rigid3DPerspectiveTransform. */
  const OffsetType & GetOffset() const
    { return m_Offset; }

  /** Get rotation from an Rigid3DPerspectiveTransform.
   * This method returns the value of the rotation of the
   * Rigid3DPerspectiveTransform. */
  const VersorType & GetRotation() const
    { return m_Versor; }

  /** Set the transformation from a container of parameters.
   * This is typically used by optimizers.
   * There are 6 parameters. The first three represent the
   * versor and the last three represents the offset. */
  void SetParameters( const ParametersType & parameters );

  /** This method sets the offset of an Rigid3DPerspectiveTransform to a
   * value specified by the user. */
  void SetOffset(const OffsetType &offset)
    { m_Offset = offset; return; }

  /** This method sets the rotation of an Rigid3DPerspectiveTransform to a
   * value specified by the user.  */
  void SetRotation(const VersorType &rotation);

  /** Set Rotation of the Rigid transform.
   * This method sets the rotation of an Rigid3DTransform to a
   * value specified by the user using the axis of rotation an
   * the angle. */
  void SetRotation(const Vector<TScalarType,3> &axis, double angle);

  /** Set the Focal Distance of the projection
   * This method sets the focal distance for the perspective
   * projection to a value specified by the user. */
  void SetFocalDistance( TScalarType focalDistance )
    { m_FocalDistance = focalDistance; }

  /** Set the Height of the output plan
   * This method sets the height of the output plan
   * to a value specified by the user.
   * This value is used to scale and center the points 
   * at the output. */
  void SetHeight( TScalarType height )
    { m_Height = height; }

  /** Set the Width of the output plan
   * This method sets the width of the output plan
   * to a value specified by the user.
   * This value is used to scale and center the points 
   * at the output. */
  void SetWidth( TScalarType width )
    { m_Width = width; }

  /** Transform by a Rigid3DPerspectiveTransform. This method 
   *  applies the transform given by self to a
   *  given point, returning the transformed point. */
  OutputPointType  TransformPoint(const InputPointType  &point ) const;

  /** Compute the matrix. */
  void ComputeMatrix(void);

protected:
    Rigid3DPerspectiveTransform();
    ~Rigid3DPerspectiveTransform();
    void PrintSelf(std::ostream &os, Indent indent) const;

private:
  Rigid3DPerspectiveTransform(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Offset of the transformation. */
  OffsetType          m_Offset;   

  /** Rotation of the transformation. */
  VersorType          m_Versor; 

  /** Set Focal distance of the projection. */
  TScalarType         m_FocalDistance;  

  /** Set the height of the output plan. */
  TScalarType         m_Height;  

  /** Set the width of the output plan. */
  TScalarType         m_Width;  

  /** Matrix representation of the rotation. */
  MatrixType          m_DirectMatrix;   

}; //class Rigid3DPerspectiveTransform:



}  // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRigid3DPerspectiveTransform.txx"
#endif

#endif /* __itkRigid3DPerspectiveTransform_h */
