/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkQuaternionRigidTransformTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>

#include "itkQuaternionRigidTransform.h"
#include "vnl/vnl_vector_fixed.h"
#include "itkVector.h"




int itkQuaternionRigidTransformTest(int ,char * [] )
{


  typedef itk::QuaternionRigidTransform<double>  TransformType;

  const double epsilon = 1e-10;
  const unsigned int N = 3;


  bool Ok = true;

  /* Create a 3D identity transformation and show its parameters */
  {
    TransformType::Pointer  identityTransform = TransformType::New();
    TransformType::OffsetType offset = identityTransform->GetOffset();
    std::cout << "Vector from instantiating an identity transform:  ";
    std::cout << offset << std::endl;

    for(unsigned int i=0; i<N; i++)
    {
      if( fabs( offset[i]-0.0 ) > epsilon )
      {
        Ok = false;
        break;    
      }
    }
    if( !Ok )
    { 
      std::cerr << "Identity doesn't have a null offset" << std::endl;
      return EXIT_FAILURE;
    }
  }


 
  /* Create a Rigid 3D transform with translation */
  {
    TransformType::Pointer  translation = TransformType::New();
    TransformType::OffsetType::ValueType ioffsetInit[3] = {1,4,9};
    TransformType::OffsetType ioffset = ioffsetInit;

    translation->SetOffset( ioffset );

    std::cout << "translation: " << translation;

    TransformType::OffsetType offset = translation->GetOffset();
    std::cout << "pure Translation test:  ";
    std::cout << offset << std::endl;

    for(unsigned int i=0; i<N; i++)
    {
      if( fabs( offset[i]- ioffset[i] ) > epsilon )
      {
        Ok = false;
        break;    
      }
    }
    if( !Ok )
    { 
      std::cerr << "Get Offset  differs from SetOffset value " << std::endl;
      return EXIT_FAILURE;
    }

    {
      // Translate an itk::Point
      TransformType::InputPointType::ValueType pInit[3] = {10,10,10};
      TransformType::InputPointType p = pInit;
      TransformType::InputPointType q;
      q = p + ioffset;
      TransformType::OutputPointType r;
      r = translation->TransformPoint( p );
      for(unsigned int i=0; i<N; i++)
      {
        if( fabs( q[i]- r[i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
      if( !Ok )
      { 
        std::cerr << "Error translating point: " << p << std::endl;
        std::cerr << "Result should be       : " << q << std::endl;
        std::cerr << "Reported Result is     : " << r << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cout << "Ok translating an itk::Point " << std::endl;
      }
    }

    {
      // Translate an itk::Vector
      TransformType::InputVectorType::ValueType pInit[3] = {10,10,10};
      TransformType::InputVectorType p = pInit;
      TransformType::OutputVectorType q;
      q = translation->TransformVector( p );
      for(unsigned int i=0; i<N; i++)
      {
        if( fabs( q[i]- p[i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
      if( !Ok )
      { 
        std::cerr << "Error translating vector: " << p << std::endl;
        std::cerr << "Reported Result is      : " << q << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cout << "Ok translating an itk::Vector " << std::endl;
      }
    }

    {
      // Translate an itk::CovariantVector
      TransformType::InputCovariantVectorType::ValueType pInit[3] = {10,10,10};
      TransformType::InputCovariantVectorType p = pInit;
      TransformType::OutputCovariantVectorType q;
      q = translation->TransformCovariantVector( p );
      for(unsigned int i=0; i<N; i++)
      {
        if( fabs( q[i]- p[i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
      if( !Ok )
      { 
        std::cerr << "Error translating covariant vector: " << p << std::endl;
        std::cerr << "Reported Result is      : " << q << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cout << "Ok translating an itk::CovariantVector " << std::endl;
      }
    }

    
    {
      // Translate a vnl_vector
      TransformType::InputVnlVectorType p;
      p[0] = 11;
      p[1] =  7;
      p[2] = 15;
      TransformType::OutputVnlVectorType q;
      q = translation->TransformVector( p );
      for(unsigned int i=0; i<N; i++)
      {
        if( fabs( q[i] - p[i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
      if( !Ok )
      { 
        std::cerr << "Error translating vnl_vector: " << p << std::endl;
        std::cerr << "Reported Result is      : " << q << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cout << "Ok translating an vnl_Vector " << std::endl;
      }
    }




  }

 
  /* Create a Rigid 3D transform with a rotation given by a Matrix */
  {
    TransformType::Pointer  rotation = TransformType::New();
    TransformType::VnlQuaternionType qrotation;
   
    // 15 degrees in radians
    const double angle = 15.0 * atan( 1.0f ) / 45.0; 
    const double sinth2 = sin( angle / 2.0 );
    const double costh2 = cos( angle / 2.0 );

    const double sinth  = sin( angle );
    const double costh  = cos( angle );

    // around the positive Z axis 
    qrotation[0] =     0.0;
    qrotation[1] =     0.0;
    qrotation[2] =  sinth2;
    qrotation[3] =  costh2;

    rotation->SetRotation( qrotation );

    TransformType::OffsetType ioffset;
    ioffset.Fill( 0.0f );

    rotation->SetOffset( ioffset );

    std::cout << "rotation: " << rotation;

    // Verify the Offset content
    TransformType::OffsetType offset = rotation->GetOffset();
    std::cout << "pure Rotation test:  " << std::endl;
    std::cout << "Offset = " << offset << std::endl;

    for(unsigned int i=0; i<N; i++)
    {
      if( fabs( offset[i]- ioffset[i] ) > epsilon )
      {
        Ok = false;
        break;    
      }
    }
    if( !Ok )
    { 
      std::cerr << "Get Offset  differs from SetOffset value " << std::endl;
      return EXIT_FAILURE;
    }

    // VNL uses transposed matrices.
    vnl_matrix_fixed<double,3,3> mrotation = qrotation.rotation_matrix();

    // Verify the Matrix content
    TransformType::MatrixType matrix = rotation->GetRotationMatrix();
    std::cout << "Rotation matrix:  " << std::endl;
    std::cout << matrix << std::endl;

    for(unsigned int i=0; i<N; i++)
    {
      for(unsigned int j=0; j<N; j++)
      {
        if( fabs( matrix[i][j]- mrotation[j][i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
    }
    if( !Ok )
    { 
      std::cerr << "Get Rotation Matrix  differs " << std::endl;
      std::cerr << "from SetRotationMatrix value " << std::endl;
      return EXIT_FAILURE;
    }

    {
      // Rotate an itk::Point
      TransformType::InputPointType::ValueType pInit[3] = {10,10,10};
      TransformType::InputPointType p = pInit;
      TransformType::InputPointType q;

      q[0] =  p[0] * costh - p[1] * sinth;
      q[1] =  p[0] * sinth + p[1] * costh;
      q[2] =  p[2];

      TransformType::OutputPointType r;
      r = rotation->TransformPoint( p );
      for(unsigned int i=0; i<N; i++)
      {
        if( fabs( q[i]- r[i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
      if( !Ok )
      { 
        std::cerr << "Error rotating point   : " << p << std::endl;
        std::cerr << "Result should be       : " << q << std::endl;
        std::cerr << "Reported Result is     : " << r << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cout << "Ok translating an itk::Point " << std::endl;
      }
    }

    {
      // Rotate an itk::Vector
      TransformType::InputVectorType::ValueType pInit[3] = {10,10,10};
      TransformType::InputVectorType p = pInit;

      TransformType::InputPointType q;
      q[0] =  p[0] * costh - p[1] * sinth;
      q[1] =  p[0] * sinth + p[1] * costh;
      q[2] =  p[2];

      TransformType::OutputVectorType r;
      r = rotation->TransformVector( p );
      for(unsigned int i=0; i<N; i++)
      {
        if( fabs( q[i] - r[i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
      if( !Ok )
      { 
        std::cerr << "Error rotating vector  : " << p << std::endl;
        std::cerr << "Result should be       : " << q << std::endl;
        std::cerr << "Reported Result is     : " << r << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cout << "Ok rotating an itk::Vector " << std::endl;
      }
    }

    {
      // Rotate an itk::CovariantVector
      TransformType::InputCovariantVectorType::ValueType pInit[3] = {10,10,10};
      TransformType::InputCovariantVectorType p = pInit;
      TransformType::OutputCovariantVectorType q;

      q[0] =  p[0] * costh - p[1] * sinth;
      q[1] =  p[0] * sinth + p[1] * costh;
      q[2] =  p[2];

      TransformType::OutputCovariantVectorType r;
      r = rotation->TransformCovariantVector( p );

      for(unsigned int i=0; i<N; i++)
      {
        if( fabs( q[i] - r[i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
      if( !Ok )
      { 
        std::cerr << "Error Rotating covariant vector: " << p << std::endl;
        std::cerr << "Result should be               : " << q << std::endl;
        std::cerr << "Reported Result is             : " << r << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cout << "Ok translating an itk::CovariantVector " << std::endl;
      }
    }

    
    {
      // Translate a vnl_vector
      TransformType::InputVnlVectorType p;
      p[0] = 11;
      p[1] =  7;
      p[2] = 15;

      TransformType::OutputVnlVectorType q;

      q[0] =  p[0] * costh - p[1] * sinth;
      q[1] =  p[0] * sinth + p[1] * costh;
      q[2] =  p[2];


      TransformType::OutputVnlVectorType r;
      r = rotation->TransformVector( p );
      for(unsigned int i=0; i<N; i++)
      {
        if( fabs( q[i] - r[i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
      if( !Ok )
      { 
        std::cerr << "Error translating vnl_vector : " << p << std::endl;
        std::cerr << "Result should be             : " << q << std::endl;
        std::cerr << "Reported Result is           : " << r << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cout << "Ok translating an vnl_Vector " << std::endl;
      }
    }

  }

    {
    // Test the Jacobian
    std::cout << "Testing GetJacobian()" << std::endl;

    TransformType::Pointer quaternionRigid = TransformType::New();
    TransformType::ParametersType parameters( quaternionRigid->GetNumberOfParameters() );

    parameters.Fill( 0.0 );

    double angle = 0.62 / 180.0 * vnl_math::pi;

    parameters[0] =  2.0 * sin( 0.5 * angle );
    parameters[1] =  5.0 * sin( 0.5 * angle );
    parameters[2] = -4.0 * sin( 0.5 * angle );
    parameters[3] =        cos( 0.5 * angle );
    parameters[4] = 6.0;
    parameters[5] = 8.0;
    parameters[6] = 10.0;

    quaternionRigid->SetParameters( parameters );
    
    TransformType::InputPointType pInit;
    pInit[0] = 1.0;
    pInit[1] = 1.5;
    pInit[2] = 2.6;

    TransformType::JacobianType jacobian;
    jacobian = quaternionRigid->GetJacobian( pInit );
    std::cout << jacobian << std::endl;

    TransformType::JacobianType approxJacobian = jacobian;

    for( unsigned int k = 0; k < quaternionRigid->GetNumberOfParameters(); k++ )
      {
      const double delta = 0.001;
      TransformType::ParametersType plusParameters;
      TransformType::ParametersType minusParameters;

      plusParameters = parameters;
      minusParameters = parameters;
      plusParameters[k] += delta;
      minusParameters[k] -= delta;

      TransformType::OutputPointType plusPoint;
      TransformType::OutputPointType minusPoint;

      quaternionRigid->SetParameters( plusParameters );
      plusPoint = quaternionRigid->TransformPoint( pInit );
      quaternionRigid->SetParameters( minusParameters );
      minusPoint = quaternionRigid->TransformPoint( pInit );

      for( unsigned int j = 0; j < 3; j++ )
        {
        double approxDerivative = ( plusPoint[j] - minusPoint[j] ) / ( 2.0 * delta );
        double computedDerivative = jacobian[j][k];
        approxJacobian[j][k] = approxDerivative;
        if ( vnl_math_abs( approxDerivative - computedDerivative ) > 1e-5 )
          {
          std::cerr << "Error computing Jacobian [" << j << "][" << k << "]" << std::endl;
          std::cerr << "Result should be: " << approxDerivative << std::endl;
          std::cerr << "Reported result is: " << computedDerivative << std::endl;
          std::cerr << " [ FAILED ] " << std::endl;
          return EXIT_FAILURE;
          } // if
        } // for j

      } // for k

    std::cout << approxJacobian << std::endl;
    std::cout << " [ PASSED ] " << std::endl;

    // Testing inverse transform
    std::cout << "Testing BackTransform()" << std::endl;
    TransformType::OutputPointType pOut;
    quaternionRigid->SetParameters( parameters );
    pOut = quaternionRigid->BackTransform( quaternionRigid->TransformPoint( pInit ) );  

    // pOut should equate pInit
    for( unsigned int j = 0; j < 3; j++ )
      {
      if ( vnl_math_abs( pOut[j] - pInit[j] ) > 1e-5 )
        {
        std::cerr << "Error computing back transform" << std::endl;
        std::cerr << "Result should be: " << pInit << std::endl;
        std::cerr << "Reported result is: " << pOut << std::endl;
        std::cerr << " [ FAILED ] " << std::endl;
        return EXIT_FAILURE;
        }
      }

    std::cout << " [ PASSED ] " << std::endl;

    } 

 
  /* Create a Rigid 3D transform with a defined center and a rotation given by a Matrix */
  {
    TransformType::Pointer  rotation = TransformType::New();
    TransformType::VnlQuaternionType qrotation;
   
    // 15 degrees in radians
    const double angle = 15.0 * atan( 1.0f ) / 45.0; 
    const double sinth2 = sin( angle / 2.0 );
    const double costh2 = cos( angle / 2.0 );

    const double sinth  = sin( angle );
    const double costh  = cos( angle );

    // around the positive Z axis 
    qrotation[0] =     0.0;
    qrotation[1] =     0.0;
    qrotation[2] =  sinth2;
    qrotation[3] =  costh2;

    rotation->SetRotation( qrotation );

    TransformType::InputPointType  center;
    center[0] = 17;
    center[1] = 19;
    center[2] = 23;


    TransformType::OffsetType ioffset;
    ioffset.Fill( 0.0f );

    rotation->SetOffset( ioffset );

    
    rotation->SetCenter( center );

    std::cout << "rotation: " << rotation;

    // Verify the Offset content
    TransformType::OffsetType offset = rotation->GetOffset();
    std::cout << "pure Rotation test:  " << std::endl;
    std::cout << "Offset = " << offset << std::endl;

    for(unsigned int i=0; i<N; i++)
    {
      if( fabs( offset[i]- ioffset[i] ) > epsilon )
      {
        Ok = false;
        break;    
      }
    }
    if( !Ok )
    { 
      std::cerr << "Get Offset  differs from SetOffset value " << std::endl;
      return EXIT_FAILURE;
    }

    // VNL uses transposed matrices.
    vnl_matrix_fixed<double,3,3> mrotation = qrotation.rotation_matrix();

    // Verify the Matrix content
    TransformType::MatrixType matrix = rotation->GetRotationMatrix();
    std::cout << "Rotation matrix:  " << std::endl;
    std::cout << matrix << std::endl;

    for(unsigned int i=0; i<N; i++)
    {
      for(unsigned int j=0; j<N; j++)
      {
        if( fabs( matrix[i][j]- mrotation[j][i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
    }
    if( !Ok )
    { 
      std::cerr << "Get Rotation Matrix  differs " << std::endl;
      std::cerr << "from SetRotationMatrix value " << std::endl;
      return EXIT_FAILURE;
    }

    {
      // Rotate an itk::Point
      TransformType::InputPointType::ValueType pInit[3] = {10,10,10};
      TransformType::InputPointType p = pInit;
      TransformType::InputPointType q;

      q[0] =  p[0] * costh - p[1] * sinth;
      q[1] =  p[0] * sinth + p[1] * costh;
      q[2] =  p[2];

      TransformType::OutputPointType r;
      r = rotation->TransformPoint( p );
      for(unsigned int i=0; i<N; i++)
      {
        if( fabs( q[i]- r[i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
      if( !Ok )
      { 
        std::cerr << "Error rotating point   : " << p << std::endl;
        std::cerr << "Result should be       : " << q << std::endl;
        std::cerr << "Reported Result is     : " << r << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cout << "Ok translating an itk::Point " << std::endl;
      }
    }

    {
      // Rotate an itk::Vector
      TransformType::InputVectorType::ValueType pInit[3] = {10,10,10};
      TransformType::InputVectorType p = pInit;

      TransformType::InputPointType q;
      q[0] =  p[0] * costh - p[1] * sinth;
      q[1] =  p[0] * sinth + p[1] * costh;
      q[2] =  p[2];

      TransformType::OutputVectorType r;
      r = rotation->TransformVector( p );
      for(unsigned int i=0; i<N; i++)
      {
        if( fabs( q[i] - r[i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
      if( !Ok )
      { 
        std::cerr << "Error rotating vector  : " << p << std::endl;
        std::cerr << "Result should be       : " << q << std::endl;
        std::cerr << "Reported Result is     : " << r << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cout << "Ok rotating an itk::Vector " << std::endl;
      }
    }

    {
      // Rotate an itk::CovariantVector
      TransformType::InputCovariantVectorType::ValueType pInit[3] = {10,10,10};
      TransformType::InputCovariantVectorType p = pInit;
      TransformType::OutputCovariantVectorType q;

      q[0] =  p[0] * costh - p[1] * sinth;
      q[1] =  p[0] * sinth + p[1] * costh;
      q[2] =  p[2];

      TransformType::OutputCovariantVectorType r;
      r = rotation->TransformCovariantVector( p );

      for(unsigned int i=0; i<N; i++)
      {
        if( fabs( q[i] - r[i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
      if( !Ok )
      { 
        std::cerr << "Error Rotating covariant vector: " << p << std::endl;
        std::cerr << "Result should be               : " << q << std::endl;
        std::cerr << "Reported Result is             : " << r << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cout << "Ok translating an itk::CovariantVector " << std::endl;
      }
    }

    
    {
      // Translate a vnl_vector
      TransformType::InputVnlVectorType p;
      p[0] = 11;
      p[1] =  7;
      p[2] = 15;

      TransformType::OutputVnlVectorType q;

      q[0] =  p[0] * costh - p[1] * sinth;
      q[1] =  p[0] * sinth + p[1] * costh;
      q[2] =  p[2];


      TransformType::OutputVnlVectorType r;
      r = rotation->TransformVector( p );
      for(unsigned int i=0; i<N; i++)
      {
        if( fabs( q[i] - r[i] ) > epsilon )
        {
          Ok = false;
          break;    
        }
      }
      if( !Ok )
      { 
        std::cerr << "Error translating vnl_vector : " << p << std::endl;
        std::cerr << "Result should be             : " << q << std::endl;
        std::cerr << "Reported Result is           : " << r << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        std::cout << "Ok translating an vnl_Vector " << std::endl;
      }
    }

  }

  return EXIT_SUCCESS;

}
