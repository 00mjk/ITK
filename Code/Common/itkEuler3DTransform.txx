/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkEuler3DTransform.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkEuler3DTransform_txx
#define _itkEuler3DTransform_txx

#include "itkEuler3DTransform.h"


namespace itk
{

// Constructor with default arguments
template <class TScalarType>
Euler3DTransform<TScalarType>
::Euler3DTransform()
{
  m_ComputeZYX = false;
  m_AngleX = m_AngleY = m_AngleZ = 0.0;
}

// Copy Constructor
template <class TScalarType>
Euler3DTransform<TScalarType>
::Euler3DTransform( const Self & other )
{
  // call the superclass copy constructor
}

// Set Parameters
template <class TScalarType>
void
Euler3DTransform<TScalarType>
::SetParameters( const ParametersType & parameters )
{
  itkDebugMacro( << "Setting paramaters " << parameters );

  // Set angles with parameters
  m_AngleX = parameters[0];
  m_AngleY = parameters[1];
  m_AngleZ = parameters[2];
 
  // Transfer the translation part
  OffsetType offset;
  for(unsigned int i=0; i < SpaceDimension; i++) 
    {
    offset[i] = parameters[i+3];
    }

  this->SetOffset( offset );

  ComputeMatrix();

  itkDebugMacro(<<"After setting paramaters ");
}


// Get Parameters
template <class TScalarType>
const typename Euler3DTransform<TScalarType>::ParametersType &
Euler3DTransform<TScalarType>
::GetParameters( void ) const
{

  m_Parameters[0] = m_AngleX;
  m_Parameters[1] = m_AngleY;
  m_Parameters[2] = m_AngleZ;
  for( unsigned int i=0; i < SpaceDimension; i++ )
    {
    m_Parameters[i+3] = this->GetOffset()[i];
    }

  return m_Parameters;

}

// Set Rotational Part
template <class TScalarType>
void
Euler3DTransform<TScalarType>
::SetRotation(TScalarType angleX,TScalarType angleY,TScalarType angleZ)
{
  m_AngleX = angleX;
  m_AngleY = angleY;
  m_AngleZ = angleZ;
  ComputeMatrix();
}


// Set the rotation matrix
template <class TScalarType>
void
Euler3DTransform<TScalarType>
::SetRotationMatrix(const MatrixType &matrix)
{
  Superclass::SetRotationMatrix(matrix);
  this->ComputeAnglesFromMatrix();
}

// Compose
template <class TScalarType>
void
Euler3DTransform<TScalarType>
::Compose(const Superclass *other, bool pre)
{
  Superclass::Compose(other,pre);
  this->ComputeAnglesFromMatrix();
}
  
// Compose
template <class TScalarType>
void
Euler3DTransform<TScalarType>
::SetIdentity(void)
{
  Superclass::SetIdentity();
  m_AngleX = 0;
  m_AngleY = 0;
  m_AngleZ = 0;
}


// Compute angles from the rotation matrix
template <class TScalarType>
void
Euler3DTransform<TScalarType>
::ComputeAnglesFromMatrix(void)
{
  m_AngleX = asin(m_RotationMatrix[2][1]);
  double A = cos(m_AngleX);
  if(fabs(A)>0.00005)
    {
    double x = m_RotationMatrix[2][2] / A;
    double y = -m_RotationMatrix[2][0] / A;
    m_AngleY = atan2(y,x);

    x = m_RotationMatrix[1][1] / A;
    y = -m_RotationMatrix[0][1] / A;
    m_AngleZ = atan2(y,x);
    }
  else
    {
    m_AngleZ = 0;
    double x = m_RotationMatrix[0][0];
    double y = m_RotationMatrix[1][0];
    m_AngleY = atan2(y,x);
    }
}


// Compute the matrix
template <class TScalarType>
void
Euler3DTransform<TScalarType>
::ComputeMatrix( void )
{
  // need to check if angles are in the right order
  const double cx = cos(m_AngleX);
  const double sx = sin(m_AngleX);
  const double cy = cos(m_AngleY);
  const double sy = sin(m_AngleY); 
  const double cz = cos(m_AngleZ);
  const double sz = sin(m_AngleZ);

  Matrix<TScalarType,3,3> RotationX;
  RotationX[0][0]=1;RotationX[0][1]=0;RotationX[0][2]=0;
  RotationX[1][0]=0;RotationX[1][1]=cx;RotationX[1][2]=-sx;
  RotationX[2][0]=0;RotationX[2][1]=sx;RotationX[2][2]=cx;


  Matrix<TScalarType,3,3> RotationY;
  RotationY[0][0]=cy;RotationY[0][1]=0;RotationY[0][2]=sy;
  RotationY[1][0]=0;RotationY[1][1]=1;RotationY[1][2]=0;
  RotationY[2][0]=-sy;RotationY[2][1]=0;RotationY[2][2]=cy;

  
  Matrix<TScalarType,3,3> RotationZ;
  RotationZ[0][0]=cz;RotationZ[0][1]=-sz;RotationZ[0][2]=0;
  RotationZ[1][0]=sz;RotationZ[1][1]=cz;RotationZ[1][2]=0;
  RotationZ[2][0]=0;RotationZ[2][1]=0;RotationZ[2][2]=1;

  /** Aply the rotation first around Y then X then Z */
  if(m_ComputeZYX)
    {
    m_RotationMatrix = RotationZ*RotationY*RotationX;
    }
  else
    {
    m_RotationMatrix = RotationZ*RotationX*RotationY; // Like VTK transformation order
    }

  m_InverseMatrix = m_RotationMatrix.GetTranspose();

}


// Set parameters
template<class TScalarType>
const typename Euler3DTransform<TScalarType>::JacobianType &
Euler3DTransform<TScalarType>::
GetJacobian( const InputPointType & p ) const
{
  // need to check if angles are in the right order
  const double cx = cos(m_AngleX);
  const double sx = sin(m_AngleX);
  const double cy = cos(m_AngleY);
  const double sy = sin(m_AngleY); 
  const double cz = cos(m_AngleZ);
  const double sz = sin(m_AngleZ);

  m_Jacobian.Fill(0.0);

  if ( m_ComputeZYX )
    {
    m_Jacobian[0][0] = (cz*sy*cx+sz*sx)*p[1]+(-cz*sy*sx+sz*cx)*p[2];
    m_Jacobian[1][0] = (sz*sy*cx-cz*sx)*p[1]+(-sz*sy*sx-cz*cx)*p[2];
    m_Jacobian[2][0] = (cy*cx)*p[1]+(-cy*sx)*p[2];  
    
    m_Jacobian[0][1] = (-cz*sy)*p[0]+(cz*cy*sx)*p[1]+(cz*cy*cx)*p[2];
    m_Jacobian[1][1] = (-sz*sy)*p[0]+(sz*cy*sx)*p[1]+(sz*cy*cx)*p[2];
    m_Jacobian[2][1] = (-cy)*p[0]+(-sy*sx)*p[1]+(-sy*cx)*p[2];
    
    m_Jacobian[0][2] = (-sz*cy)*p[0]+(-sz*sy*sx-cz*cx)*p[1]+(-sz*sy*cx+cz*sx)*p[2];
    m_Jacobian[1][2] = (cz*cy)*p[0]+(cz*sy*sx-sz*cx)*p[1]+(cz*sy*cx+sz*sx)*p[2];  
    m_Jacobian[2][2] = 0;
    }
  else
    {
    m_Jacobian[0][0] = (-sz*cx*sy)*p[0] + (sz*sx)*p[1] + (sz*cx*cy)*p[2];
    m_Jacobian[1][0] = (cz*cx*sy)*p[0] + (-cz*sx)*p[1] + (-cz*cx*cy)*p[2];
    m_Jacobian[2][0] = (sx*sy)*p[0] + (cx)*p[1] + (-sx*cy)*p[2];  
    
    m_Jacobian[0][1] = (-cz*sy-sz*sx*cy)*p[0] + (cz*cy-sz*sx*sy)*p[2];
    m_Jacobian[1][1] = (-sz*sy+cz*sx*cy)*p[0] + (sz*cy+cz*sx*sy)*p[2];
    m_Jacobian[2][1] = (-cx*cy)*p[0] + (-cx*sy)*p[2];
    
    m_Jacobian[0][2] = (-sz*cy-cz*sx*sy)*p[0] + (-cz*cx)*p[1] + (-sz*sy+cz*sx*cy)*p[2];
    m_Jacobian[1][2] = (cz*cy-sz*sx*sy)*p[0] + (-sz*cx)*p[1] + (cz*sy+sz*sx*cy)*p[2];
    m_Jacobian[2][2] = 0;
    }
 
  // compute derivatives for the translation part
  unsigned int blockOffset = 3;  
  for(unsigned int dim=0; dim < SpaceDimension; dim++ ) 
    {
    m_Jacobian[ dim ][ blockOffset + dim ] = 1.0;
    }

  return m_Jacobian;

}
  
// Print self
template<class TScalarType>
void
Euler3DTransform<TScalarType>::
PrintSelf(std::ostream &os, Indent indent) const
{

  Superclass::PrintSelf(os,indent);

  os << indent << "Euler's angles: AngleX=" << m_AngleX  
     << " AngleY=" << m_AngleY  
     << " AngleZ=" << m_AngleZ  
     << std::endl;

  os << indent << "m_ComputeZYX = " << m_ComputeZYX << std::endl;
}

} // namespace

#endif
