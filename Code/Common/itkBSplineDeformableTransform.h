/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBSplineDeformableTransform.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkBSplineDeformableTransform_h
#define __itkBSplineDeformableTransform_h

#include <iostream>
#include "itkTransform.h"
#include "itkImage.h"
#include "itkImageRegion.h"
#include "itkBSplineKernelFunction.h"

namespace itk
{

/** \class BSplineDeformableTransform
 * \brief Deformable transform using a BSpline representation
 *
 * This class encapsulates a deformable transform of point from one N-dimensional
 * one space to another N-dimensional space.
 * The deformation field is modeled using BSplines. 
 * A deformation is defined on a sparse regular grid of control points
 * \f$ \vec{\lambda}_j \f$ and is varied by defining a deformation 
 * \f$ \vec{g}(\vec{\lambda}_j) \f$ of each control point. 
 * The deformation \f$ D(\vec{x}) \f$ at any point \f$ \vec{x} \f$
 * is obtained by using a BSpline interpolation kernel.
 *
 * The deformation field grid is defined by a user specified GridRegion, 
 * GridSpacing and GridOrigin. Each grid/control point has associated with it
 * N deformation coefficients \f$ \vec{\delta}_j \f$, representing the N directional 
 * components of the deformation. Deformation outside the grid plus support
 * region for the BSpline interpolation is assumed to be zero.
 *
 * Additionally, the user can specified an addition bulk transform \f$ B \f$
 * such that the transformed point is given by:
 * \f[ \vec{y} = B(\vec{x}) + D(\vec{x}) \f]
 *
 * The parameters for this transform is N x N-D grid of spline coefficients.
 * The user specifies the parameters as one flat array: each N-D grid
 * is represented by an array in the same way an N-D image is represented
 * in the buffer; the N arrays are then concatentated together on form
 * a single array.
 *
 * For efficiency, this transform does not make a copy of the parameters.
 * It only keeps a pointer to the input parameters and assumes that the memory
 * is managed by the caller.
 * 
 * The class is templated coordinate representation type (float or double),
 * the space dimension and the spline order.
 *
 * \ingroup Transforms
 */
template <
    class TScalarType = double,          // Data type for scalars (float or double)
    unsigned int NDimensions = 3,        // Number of dimensions
    unsigned int VSplineOrder = 3 >      // Spline order
class BSplineDeformableTransform : 
          public Transform< TScalarType, NDimensions, NDimensions >
{
public:
  /** Standard class typedefs. */
  typedef BSplineDeformableTransform Self;
  typedef Transform< TScalarType, NDimensions, NDimensions > Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
      
  /** New macro for creation of through the object factory.*/
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( BSplineDeformableTransform, Transform );

  /** Dimension of the domain space. */
  enum { SpaceDimension  = NDimensions };

  /** The BSpline order. */
  enum { SplineOrder = VSplineOrder };

  /** Standard scalar type for this class. */
  typedef typename Superclass::ScalarType ScalarType;

  /** Standard parameters container. */
  typedef typename Superclass::ParametersType ParametersType;

  /** Standard Jacobian container. */
  typedef typename Superclass::JacobianType JacobianType;

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
  
  /** This method sets the parameters of the transform.
   * For a BSpline deformation transform, the parameters are the BSpline 
   * coefficients on a sparse grid. 
   * 
   * The parameters are N number of N-D grid of coefficients. Each N-D grid 
   * is represented as a flat array of doubles (in the same configuration as an itk::Image).
   * The N arrays are then concatenated to form one parameter array.
   *
   * For efficiency, this transform does not make a copy of the parameters.
   * It only keeps a pointer to the input parameters. It assumes that the memory
   * is managed by the caller.
   *
   * This method wraps each grid as itk::Image's using the user specified
   * grid region, spacing and origin.
   * NOTE: The grid region, spacing and origin must be set first.
   *
   */
  void SetParameters(const ParametersType & parameters);

  /** Get the Transformation Parameters. */
  virtual const ParametersType& GetParameters(void) const;

  /** Typedefs for specificing the extend to the grid. */
  typedef ImageRegion<SpaceDimension>    RegionType;
  typedef typename RegionType::IndexType IndexType;
  typedef typename RegionType::SizeType  SizeType;
  typedef FixedArray<double> SpacingType;
  typedef FixedArray<double> OriginType;

  /** This method specifies the region over which the grid resides. */
  virtual void SetGridRegion( const RegionType& region );
  itkGetMacro( GridRegion, RegionType );

  /** This method specifies the grid spacing or resolution. */
  virtual void SetGridSpacing( const SpacingType& spacing );
  itkGetMacro( GridSpacing, SpacingType );

  /** This method specifies the grid origin. */
  virtual void SetGridOrigin( const OriginType& origin );
  itkGetMacro( GridOrigin, OriginType );

  /** Typedef of the bulk transform. */
  typedef Transform<ScalarType,SpaceDimension,SpaceDimension> BulkTransformType;
  typedef typename BulkTransformType::ConstPointer            BulkTransformPointer;

  /** This method specifies the bulk transform to be applied. 
   * The default is the identity transform.
   */
  itkSetConstObjectMacro( BulkTransform, BulkTransformType );
  itkGetConstObjectMacro( BulkTransform, BulkTransformType );

  /** Transform points by an BSpline deformable transformation. */
  OutputPointType  TransformPoint(const InputPointType  &point ) const;

  /**  Method to transform a vector - not applicable for this type of transform. */
  virtual OutputVectorType    TransformVector(const InputVectorType &vector) const
    { 
      itkExceptionMacro( << "Method not applicable for deformable transform." );
      return OutputVectorType(); 
    }

  /**  Method to transform a vnl_vector - not applicable for this type of transform */
  virtual OutputVnlVectorType TransformVector(const InputVnlVectorType &vector) const
    { 
      itkExceptionMacro( << "Method not applicable for deformable transform. " );
      return OutputVnlVectorType(); 
    }

  /**  Method to transform a CovariantVector - not application for this type of transform */
  virtual OutputCovariantVectorType TransformCovariantVector(
    const InputCovariantVectorType &vector) const
    { 
      itkExceptionMacro( << "Method not applicable for deformable transfrom. " );
      return OutputCovariantVectorType(); 
    } 
    
  /** Print contents of an BSplineDeformableTransform. */
  void PrintSelf(std::ostream &os, Indent indent) const;

  /** Compute the Jacobian Matrix of the transformation at one point */
  virtual const JacobianType & GetJacobian(const InputPointType  &point ) const;

  /** Return the number of parameters that completely define the Transfom */
  unsigned int virtual GetNumberOfParameters(void) const;

protected:
  BSplineDeformableTransform();
  ~BSplineDeformableTransform();

private:
  BSplineDeformableTransform(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // The bulk transform
  BulkTransformPointer  m_BulkTransform;

  // Variables defining the coefficient grid extend
  RegionType    m_GridRegion;
  SpacingType   m_GridSpacing;
  OriginType    m_GridOrigin;
  
  RegionType    m_ValidRegion;

  // Variables defining the interpolation support region
  unsigned long m_Offset;
  SizeType      m_SupportSize;

  // Parameters as SpaceDimension number of images
  typedef typename ParametersType::ValueType PixelType;
  typedef Image<PixelType,SpaceDimension> ImageType;
  
  typename ImageType::Pointer   m_CoefficientImage[SpaceDimension];

  // Jacobian as SpaceDimension number of images
  typedef typename JacobianType::ValueType JacobianPixelType;
  typedef Image<JacobianPixelType,SpaceDimension> JacobianImageType;
 
  typename JacobianImageType::Pointer m_JacobianImage[SpaceDimension];

  // Keep track of last support region used in computing the Jacobian
  // for fast resetting of Jacobian to zero
  mutable IndexType m_LastJacobianIndex;

  // Keep a pointer to the input parameters
  const ParametersType *  m_InputParametersPointer;

  // BSpline kernel function for reconstruction
  typedef BSplineKernelFunction<SplineOrder> KernelType;
  typename KernelType::Pointer  m_Kernel;

  // Compute interpolation weights 
  typedef vnl_matrix_fixed<double,SpaceDimension,SplineOrder + 1> WeightsType;
  void ComputeInterpolationWeights( const InputPointType & point, 
    WeightsType & weights, IndexType & supportIndex, bool & valid ) const;

  // Table mapping support region offset to index
  typedef vnl_matrix<unsigned long> TableType;
  TableType m_SupportOffsetToIndexTable;


}; //class BSplineDeformableTransform


}  // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBSplineDeformableTransform.txx"
#endif

#endif /* __itkBSplineDeformableTransform_h */
