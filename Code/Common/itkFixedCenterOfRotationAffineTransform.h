/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFixedCenterOfRotationAffineTransform.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkFixedCenterOfRotationAffineTransform_h
#define __itkFixedCenterOfRotationAffineTransform_h

#include "itkAffineTransform.h"

namespace itk
{


/**
 * \brief Affine transformation with a specified center of rotation.
 *
 * This class implements an Affine transform in which the rotation center can be explicitly selected.
 *
 * 
 * \ingroup Transforms
 *
 *
 */

template <
 class TScalarType=double,      // Data type for scalars (e.g. float or double)
 unsigned int NDimensions=3>    // Number of dimensions in the input space
class ITK_EXPORT FixedCenterOfRotationAffineTransform 
: public AffineTransform< TScalarType, NDimensions >
{
public:
  /** Standard typedefs   */
  typedef FixedCenterOfRotationAffineTransform  Self;
  typedef AffineTransform< TScalarType, NDimensions >  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
    
  /** Run-time type information (and related methods).   */
  itkTypeMacro( FixedCenterOfRotationAffineTransform, AffineTransform );
  
  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro( Self );
  
  /** Dimension of the domain space. */
  itkStaticConstMacro(SpaceDimension, unsigned int, NDimensions);
  itkStaticConstMacro(ParametersDimension, unsigned int,
                      NDimensions*(NDimensions+1));
  
    
  /** Types taken from the Superclass */
  typedef typename Superclass::ParametersType            ParametersType;
  typedef typename Superclass::JacobianType              JacobianType;
  typedef typename Superclass::ScalarType                ScalarType;
  typedef typename Superclass::InputVectorType           InputVectorType;
  typedef typename Superclass::OutputVectorType          OutputVectorType;
  typedef typename Superclass::InputCovariantVectorType     
                                            InputCovariantVectorType;
  typedef typename Superclass::OutputCovariantVectorType    
                                            OutputCovariantVectorType;
  typedef typename Superclass::InputVnlVectorType        InputVnlVectorType;
  typedef typename Superclass::OutputVnlVectorType       OutputVnlVectorType;
  typedef typename Superclass::InputPointType            InputPointType;
  typedef typename Superclass::OutputPointType           OutputPointType;
  typedef typename Superclass::MatrixType                MatrixType;
  typedef typename Superclass::OffsetType                OffsetType;
    
  /** Set the transformation to an Identity
   *
   * This sets the matrix to identity and the Offset to null. */
  void SetIdentity( void );
  
  /** Print contents of an FixedCenterOfRotationAffineTransform */
  void PrintSelf(std::ostream &s, Indent indent) const;

  /** Set and Get the center of rotation */
  void SetCenterOfRotationComponent(const InputPointType &cor);
  itkGetConstReferenceMacro( CenterOfRotationComponent, InputPointType );
   
  /** Set the scale of the transform */
  virtual void SetScaleComponent( const double scale[NDimensions] );
  /** Get the scale of the transform*/
  virtual const double* GetScaleComponent() const {return m_ScaleComponent;};

  /** Set the matrix of the transform. The matrix should not include
   *  scale */
  void SetMatrixComponent(const MatrixType &matrix);
  /** Get matrix of the transform  */
  const MatrixType & GetMatrixComponent() const { return m_MatrixComponent; }

  /** Set offset (origin) of the Transform.*/
  void SetOffsetComponent(const OffsetType &offset);

  /** Get offset of the transform*/
  const OffsetType & GetOffsetComponent(void) const 
                                      { return m_OffsetComponent; }

  /** Set matrix of the AffineTransform
   *  This reinitializes the different components, i.e ScaleComponent,
   *  OffsetComponent, etc ...*/
  void SetMatrix(const MatrixType &matrix);

  /** Set offset (origin) of the Affine Transform.
   *  This reinitializes the different components, i.e ScaleComponent,
   *  OffsetComponent, etc ...*/
  void SetOffset(const OffsetType &offset);

  /** Set the transformation from a container of parameters.
   * The first (NDimension x NDimension) parameters define the
   * matrix and the last NDimension parameters the translation. */
  void SetParameters( const ParametersType & parameters );

  /** Get the Transformation Parameters. */
  const ParametersType& GetParameters(void) const;

protected:
  /** Construct an FixedCenterOfRotationAffineTransform object */
  FixedCenterOfRotationAffineTransform();      
   
  /** Destroy an FixedCenterOfRotationAffineTransform object   */
  virtual ~FixedCenterOfRotationAffineTransform();
    
private:

  FixedCenterOfRotationAffineTransform(const Self & other);
  const Self & operator=( const Self & );
  
  InputPointType      m_CenterOfRotationComponent;
  double              m_ScaleComponent[NDimensions];
  MatrixType          m_MatrixComponent;
  MatrixType          m_ScaleMatrixComponent;
  OffsetType          m_OffsetComponent;

  void RecomputeMatrix();
  void RecomputeOffset();


}; //class FixedCenterOfRotationAffineTransform
  
}  // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFixedCenterOfRotationAffineTransform.txx"
#endif

#endif /* __itkFixedCenterOfRotationAffineTransform_h */





