/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageToImageAffineMeanSquaresGradientDescentRegistration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef __itkImageToImageAffineMeanSquaresGradientDescentRegistration_h
#define __itkImageToImageAffineMeanSquaresGradientDescentRegistration_h

#include "itkRegistrationMethod.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkGradientDescentOptimizer.h"
#include "itkImage.h"
#include "itkImageMapper.h"
#include "itkAffineTransform.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkPoint.h"

namespace itk
{

/**
 *  Traits class that defines the different types to be
 *  used by this registration method.
 */
template <class TReference, class TTarget>
class ITK_EXPORT ImageToImageAffineMeanSquaresGradientDescentRegistrationTraits 
{
public:
  /**  Type of the reference. */
  typedef TReference  ReferenceType;

  /**  Type of the target. */
  typedef TTarget TargetType;

  /** Image dimensions. */
  enum { ImageDimension = ReferenceType::ImageDimension }; 

  /**  Type of the transformation. */
  typedef AffineTransform< double, ImageDimension > TransformationType;

  /**  Type of the parameters. */
  typedef typename TransformationType::ParametersType  ParametersType;

  /** Parameters dimensions. */
  enum { ParametersDimension = TransformationType::ParametersDimension }; 
 
  /**  Type of the mapper. */
  typedef ImageMapper<ReferenceType,TransformationType>  MapperType;

  /**  Type of the metric. */
  typedef MeanSquaresImageToImageMetric<TargetType, MapperType> MetricType;

  /**  Type of the optimizer.  */
   typedef GradientDescentOptimizer<MetricType>  OptimizerType;
};

/** \class ImageToImageAffineMeanSquaresGradientDescentRegistration
 * \brief Base class for registration methods
 *
 * This Class define the generic interface for a registration method.
 * The basic elements of a registration method are:
 *   - Metric to compare the reference and the target
 *   - Transformation used to register the reference against the target
 *   - Optimization method used to search for the best transformation
 * 
 * Registration is not limited to Images, and for this reason
 * this class is templated over the type of the reference object,
 * the target object and the transformation. This types are obtained
 * from the Metric type, to reduce the number of redundant
 * template parameters
 *
 *  \ingroup AffineImageRegistration 
 */
template <class TReference, class TTarget>
class ITK_EXPORT ImageToImageAffineMeanSquaresGradientDescentRegistration 
: public RegistrationMethod< 
            ImageToImageAffineMeanSquaresGradientDescentRegistrationTraits<
               TReference, TTarget>  >
{
public:
  /** Standard class typedefs. */
  typedef ImageToImageAffineMeanSquaresGradientDescentRegistration  Self;
  typedef RegistrationMethod< 
          ImageToImageAffineMeanSquaresGradientDescentRegistrationTraits<
                                       TReference,TTarget> > Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToImageAffineMeanSquaresGradientDescentRegistration, 
               RegistrationMethod);

  /**  Type of the reference. */
  typedef TReference ReferenceType;

  /**  Type of the target. */
  typedef TTarget   TargetType;

  /**  Type of the parameters. */
  typedef typename Superclass::ParametersType ParametersType;

  /**  Type of the transformation. */
  typedef typename Superclass::TransformationType TransformationType;
   
  /**  Type of the mapper. */
  typedef typename Superclass::MapperType    MapperType;

  /**  Type of the metric. */
  typedef typename Superclass::MetricType   MetricType;

  /**  Type of the optimizer.  */
  typedef typename Superclass::OptimizerType       OptimizerType;

  /** Method that initiates the registration. */
  void StartRegistration(void);


  /** Set the translation scale. */
  void SetTranslationScale( const double & scale )
    { m_TranslationScale = scale; }

  /**  Dimension of the images.  */
  enum { ImageDimension = ReferenceType::ImageDimension };

protected:
  ImageToImageAffineMeanSquaresGradientDescentRegistration();
  virtual ~ImageToImageAffineMeanSquaresGradientDescentRegistration();

private:
  ImageToImageAffineMeanSquaresGradientDescentRegistration(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ParametersType             m_Parameters;
  double                     m_TranslationScale;

};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageToImageAffineMeanSquaresGradientDescentRegistration.txx"
#endif

#endif



