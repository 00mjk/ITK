/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkVectorGradientNDAnisotropicDiffusionFunction.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkVectorGradientNDAnisotropicDiffusionFunction_h_
#define __itkVectorGradientNDAnisotropicDiffusionFunction_h_

#include "itkVectorAnisotropicDiffusionFunction.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkVectorNeighborhoodInnerProduct.h"
#include "itkDerivativeOperator.h"

namespace itk {

/** \class VectorGradientNDAnisotropicDiffusionFunction
 *  
 * \ingroup Operators
 *
 * \todo Convert this class to ND and write a NDGradientAnis....Function
 */ 
template <class TImage>
class VectorGradientNDAnisotropicDiffusionFunction :
    public VectorAnisotropicDiffusionFunction<TImage>
{
public:
  /** Standard class typedefs. */
  typedef VectorGradientNDAnisotropicDiffusionFunction Self;
  typedef VectorAnisotropicDiffusionFunction<TImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro( VectorGradientNDAnisotropicDiffusionFunction,
                ScalarAnisotropicDiffusionFunction );
  
  /** Inherit some parameters from the superclass type. */
  typedef typename Superclass::ImageType        ImageType;
  typedef typename Superclass::PixelType        PixelType;
  typedef typename Superclass::TimeStepType     TimeStepType;
  typedef typename Superclass::RadiusType       RadiusType;
  typedef typename Superclass::NeighborhoodType NeighborhoodType;
  typedef typename Superclass::BoundaryNeighborhoodType
          BoundaryNeighborhoodType;
  typedef typename Superclass::FloatOffsetType FloatOffsetType;

  /** Extract vector and image dimension from superclass. */
  enum { ImageDimension = Superclass::ImageDimension };
  enum { VectorDimension= Superclass::VectorDimension };

  /** Type of a value in a vector (double, float, etc.) */
  typedef typename PixelType::ValueType  ScalarValueType;

  /** Compute the equation value. */
  virtual PixelType ComputeUpdate(const NeighborhoodType &neighborhood,
                                  void * globalData,
                                  const FloatOffsetType& offset = m_ZeroOffset
                                  ) const;
  virtual PixelType ComputeUpdate(const BoundaryNeighborhoodType
                                  &neighborhood, void * globalData,
                                  const FloatOffsetType& offset = m_ZeroOffset
                                  ) const;

  /** This method is called prior to each iteration of the solver. */
  virtual void InitializeIteration()
    {
      m_K = this->GetAverageGradientMagnitudeSquared() *
        this->GetConductanceParameter() * -1.0f;
    }
  
protected:
  VectorGradientNDAnisotropicDiffusionFunction();
  ~VectorGradientNDAnisotropicDiffusionFunction() {}

private:
  VectorGradientNDAnisotropicDiffusionFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Inner product function. */
  VectorNeighborhoodInnerProduct<ImageType> m_InnerProduct;

  /** Boundary Inner product function. */
  SmartVectorNeighborhoodInnerProduct<ImageType> m_SmartInnerProduct;

  /** Slices for the ND neighborhood. */
  std::slice  x_slice[ImageDimension];
  std::slice xa_slice[ImageDimension][ImageDimension];
  std::slice xd_slice[ImageDimension][ImageDimension];

  /** Derivative operators. */
  DerivativeOperator<ScalarValueType, ImageDimension> dx_op;

  /** Modified global average gradient magnitude term. */
  ScalarValueType m_K;

  static double m_MIN_NORM;
  
  unsigned long int m_Stride[ImageDimension];
  unsigned long int m_Center;
  
};


  
}// end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVectorGradientNDAnisotropicDiffusionFunction.txx"
#endif

#endif
