/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkQuaternionRigidTransform.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkQuaternionRigidTransform_txx
#define _itkQuaternionRigidTransform_txx

#include "itkQuaternionRigidTransform.h"


namespace itk
{

// Constructor with default arguments
template <class TScalarType>
QuaternionRigidTransform<TScalarType>
::QuaternionRigidTransform():Superclass(SpaceDimension,ParametersDimension) 
{
  m_Offset.Fill( 0 );
  m_Rotation = VnlQuaternionType(0,0,0,1); // axis * sin(t/2), cos(t/2)
  m_RotationMatrix = m_Rotation.rotation_matrix();
  m_Parameters.Fill(0);
  m_Parameters[3] = 1.0;
}


// Print self
template<class TScalarType>
void
QuaternionRigidTransform<TScalarType>::
PrintSelf(std::ostream &os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Offset: " << m_Offset   << std::endl;
  os << indent << "Rotation: " << m_Rotation << std::endl;
  os << indent << "DirectMatrix: " << m_RotationMatrix   << std::endl;
  os << indent << "Parameters: " << m_Parameters   << std::endl;
}


// Set rotation
template<class TScalarType>
void
QuaternionRigidTransform<TScalarType>::
SetRotation(const VnlQuaternionType &rotation )
{
  m_Rotation        = rotation;
  m_InverseRotation = rotation.inverse();
  // this is done to compensate for the transposed representation
  // between VNL and ITK
  m_RotationMatrix  = m_InverseRotation.rotation_matrix();
  return;
}

// Transform a point
template<class TScalarType>
typename QuaternionRigidTransform<TScalarType>::OutputPointType
QuaternionRigidTransform<TScalarType>::
TransformPoint(const InputPointType &point) const 
{

  return m_RotationMatrix * point + m_Offset;

}


// Transform a vector
template<class TScalarType>
typename QuaternionRigidTransform<TScalarType>::OutputVectorType
QuaternionRigidTransform<TScalarType>::
TransformVector(const InputVectorType &vect) const 
{
  return  m_RotationMatrix * vect;
}


// Transform a vnl_vector_fixed
template<class TScalarType>
typename QuaternionRigidTransform<TScalarType>::OutputVnlVectorType
QuaternionRigidTransform<TScalarType>::
TransformVector(const InputVnlVectorType &vect) const 
{
  return  m_RotationMatrix * vect;
}


// Transform a CovariantVector
template<class TScalarType>
typename QuaternionRigidTransform<TScalarType>::OutputCovariantVectorType
QuaternionRigidTransform<TScalarType>::
TransformCovariantVector(const InputCovariantVectorType &vect) const 
{
  // Covariant vectors are transformed like contravariant
  // vectors under orthogonal transformations.
  return  m_RotationMatrix * vect;
}



// Back transform a point
template<class TScalarType>
typename QuaternionRigidTransform<TScalarType>::InputPointType
QuaternionRigidTransform<TScalarType>::
BackTransform(const OutputPointType &point) const 
{
  return m_InverseMatrix * (point - m_Offset);
}

// Back transform a vector
template<class TScalarType>
typename QuaternionRigidTransform<TScalarType>::InputVectorType
QuaternionRigidTransform<TScalarType>::
BackTransform(const OutputVectorType &vect ) const 
{
  return  m_InverseMatrix * vect;
}

// Back transform a vnl_vector
template<class TScalarType>
typename QuaternionRigidTransform<TScalarType>::InputVnlVectorType
QuaternionRigidTransform<TScalarType>::
BackTransform(const OutputVnlVectorType &vect ) const 
{
  return  m_InverseMatrix * vect;
}


// Back Transform a CovariantVector
template<class TScalarType>
typename QuaternionRigidTransform<TScalarType>::InputCovariantVectorType
QuaternionRigidTransform<TScalarType>::
BackTransform(const OutputCovariantVectorType &vect) const 
{
  return m_RotationMatrix * vect;
}


// Set Parameters
template <class TScalarType>
void
QuaternionRigidTransform<TScalarType>
::SetParameters( const ParametersType & parameters )
{
  VnlQuaternionType quaternion;
  OffsetType offset;

  // Transfer the quaternion part
  unsigned int par = 0;
  double norm = 0;

  for(unsigned int j=0; j < 4; j++) 
    {
    quaternion[j] = parameters[par];
    norm += vnl_math_sqr( quaternion[j] );
    ++par;
    }

  // Transfer the constant part
  for(unsigned int i=0; i < SpaceDimension; i++) 
    {
    offset[i] = parameters[par];
    ++par;
    }

  this->SetRotation( quaternion );
  this->SetOffset( offset );

}


// Set parameters
template<class TScalarType>
const typename QuaternionRigidTransform<TScalarType>::JacobianType &
QuaternionRigidTransform<TScalarType>::
GetJacobian( const InputPointType & p ) const
{
  

  // compute derivatives with respect to rotation
  VnlQuaternionType Q = m_InverseRotation;

  m_Jacobian.Fill(0.0);

  // compute Jacobian with respect to quaternion parameters
  m_Jacobian[0][0] = 2.0 * (  Q.x() * p[0] + Q.y() * p[1] + Q.z() * p[2]);
  m_Jacobian[0][1] = 2.0 * (- Q.y() * p[0] + Q.x() * p[1] - Q.r() * p[2]);
  m_Jacobian[0][2] = 2.0 * (- Q.z() * p[0] + Q.r() * p[1] + Q.x() * p[2]);
  m_Jacobian[0][3] = 2.0 * (  Q.r() * p[0] + Q.z() * p[1] - Q.y() * p[2]);

  m_Jacobian[1][0] = - m_Jacobian[0][1];
  m_Jacobian[1][1] =   m_Jacobian[0][0];
  m_Jacobian[1][2] = - m_Jacobian[0][3];
  m_Jacobian[1][3] =   m_Jacobian[0][2];

  m_Jacobian[2][0] = - m_Jacobian[0][2];
  m_Jacobian[2][1] =   m_Jacobian[0][3];
  m_Jacobian[2][2] =   m_Jacobian[0][0];
  m_Jacobian[2][3] = - m_Jacobian[0][1];


  // compute derivatives for the translation part
  unsigned int blockOffset = 4;  
  for(unsigned int dim=0; dim < SpaceDimension; dim++ ) 
    {
    m_Jacobian[ dim ][ blockOffset + dim ] = 1.0;
    }

  return m_Jacobian;

}
  
} // namespace

#endif
