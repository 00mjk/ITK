/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkGaussianSupervisedClassifier.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkGaussianSupervisedClassifier_h
#define _itkGaussianSupervisedClassifier_h

#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_math.h"
#include "vnl/algo/vnl_matrix_inverse.h"
//#include "vnl/vnl_numeric_limits.h"

#include "itkSupervisedClassifier.h"
#include <math.h>
#include <float.h>
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkExceptionObject.h"


namespace itk
{

/** \class GaussianSupervisedClassifier
 * \brief Implementation of a supervised classifier using a 
 * gaussian model.
 *
 * This object first requires training of a multivariate Gaussian 
 * models that calculates the mean and covariance of each class.
 * Using these estimates, an unclassified image can be classified 
 * in the maximum liklihood sense. The distances between an unclassified
 * input pixel to the the different classes is calculated (in our case
 * using the mahlanobis distance). The class with the least distance is
 * assigned as the unclassified pixel class label.
 * 
 * In the training part of the algorithm, given labeled training
 * samples, we estimate the mean and covariance matrix of the
 * multivariate Gaussian model for every class 
 * in the training set. Suppose there are Nc samples
 * Y_c = [y_1, y_2, ... , y_{Nc}]
 * in class c, where each sample is a vector,
 * the mean vector mc is calculated as mc = \sum_i^{Nc} y_i / Nc,
 * and the covariance matrix is computed as
 * (\sum_{i=1}^{Nc} y_i * y_i^T - Nc * mc * mc^T) / (Nc - 1). 
 *   
 * This function takes as input a sequence of multiband image slices and the 
 * class labels associated with the image slices in this sequence 
 * (the training set), and produces the mean vector and the covariance 
 * matrices for each class. 
 *
 * The class labels are numeric values assigned to each pixel. For N classes,
 * the class labels take values between 1 and N, and the value -1/0 is used
 * when a pixel's class is unknown. It is important that in the training set
 * none of the labels should exceed N. An exception is thrown if such a
 * condition is detected at run time. In addition, for reducing address
 * arithmetic the m_NumberOfClasses that is passed by the user is incremented
 * by 1.  This it to accomodate 0 as a class label which remains empty. Trade
 * off is in small additional memory requirement for storing Gaussian model's
 * parameters.
 * 
 * After the training is completed the pixels in an unclassified image are
 * classified based on the minimum distance criteria. 
 * Given an observation y and a certain class c represented by mean mc and
 * covariance matri cov_c, the Mahalanobis distance between y and class c is 
 * calculated as |y-mc|^T * inverse(cov_c) * |y-mc|. These distance
 * values can be used as inputs for initialization of MRF classification.
 * The class which has the minimum distance for a given pixel value is 
 * chosen as the label for the given pixel value
 *
 * This object supports data handling of multiband images. The object
 * accepts the input image in vector format only, where each pixel is a 
 * vector and each element of the vector corresponds to an entry from
 * 1 particular band of a multiband dataset. A single band image is treated 
 * as a vector image with a single element for every vector. The classified
 * image is treated as a single band scalar image.
 *
 * \ingroup SupervisedClassificationFilters */
template <class TInputImage, class TClassifiedImage>
class ITK_EXPORT GaussianSupervisedClassifier 
: public SupervisedClassifier < TInputImage, TClassifiedImage >
{
public:
  /** Standard class typedefs. */
  typedef GaussianSupervisedClassifier   Self;
  typedef SupervisedClassifier<TInputImage,TClassifiedImage> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(GaussianSupervisedClassifier,SupervisedClassifier);

  /** Type definition for the input image. */
  typedef TInputImage                           InputImageType;
  typedef typename TInputImage::Pointer         InputImagePointer;
  typedef typename TInputImage::ConstPointer    InputImageConstPointer;

  /** Type definitions for the training image pixel type. */
  typedef typename TClassifiedImage::Pointer ClassifiedImageType; 

  /** Type definitions for the training image pixel type. */
  typedef typename TClassifiedImage::PixelType TrainingPixelType;

  /** Type definitions for the classified image pixel type.
   * It has to be the same type as the training image. */
  typedef typename TClassifiedImage::PixelType ClassifiedPixelType;
  typedef typename TInputImage::PixelType      InputImagePixelType;
  typedef typename TClassifiedImage::PixelType ClassifiedImagePixelType;  
  typedef
    ImageRegionIterator< TInputImage >  InputImageIterator;
  typedef
    ImageRegionConstIterator< TInputImage >  InputImageConstIterator;
  typedef
    ImageRegionIterator< TClassifiedImage > TrainingImageIterator;

  /** Type definition for the vector associated with
   * input image pixel type. */     
  typedef typename TInputImage::PixelType  InputImageVectorType;

  /** Dimension of the each individual pixel vector. */
  enum{ VectorDimension = InputImageVectorType::VectorDimension };

  /** Virtual SetNumberOfClasses to initialize memory for the
   * probability estimation routine. */
  virtual void SetNumberOfClasses( const unsigned int num );

  /** Train multivariate Gaussian classifier. 
   *
   * Given a set of labeled training samples, estimate the mean and 
   * covariance of a multivariate Gaussian model for every class 
   * in the training set. Suppose there are Nc samples  in class c, where 
   * each sample is a vector, the mean vector m is calculated as 
   * {m1,m2,...,mc}, and the covariance  (cov) is computed as 
   * {cov1, cov2, ..., covc}. If there is no training samples in one class,
   * we output a zero mean vector and a zero covariance matrix for that class.
   *  */
  void TrainClassifier();

  /** Classify the input image using the Gaussian model generated by the 
   * TrainGaussianClassifier function. It firsts call the model training
   * function and then loops through the entire image set to classify
   * one pixel at a time using the GetPixelClassified function.  */
  void ApplyGaussianClassifier();

  /** Define a virtual Classifier function to classify the whole image. */
  virtual void ClassifyImage();

  /** Given a pixel value return the classified index of the pixel 
   * belonging to a class.
   * This function calls the GetPixelProbability function that 
   * returns the liklihood of a given pixel belonging to a particular
   * class. */
  int GetPixelClass(InputImageVectorType &inPixelVec);

  /** Given a pixel value return the probability of the pixel belonging to
   * different classes as a distance metric. The higher the distance the 
   * lower is the probability of a the pixel belonging to that class.
   *
   * In this implementation the Mahalanobis distance is used. The 
   * mahalanobis distance is calcualted as
   * |y-mc|^T * inverse(cov_c) * |y-mc|, where y stands 
   * for the pixel value, the inverse is a matrix inversion step and ^T
   * stands for the matrix transpose operation. */
  void GetPixelDistance(InputImageVectorType &inPixelVec, double * results);

protected:
  GaussianSupervisedClassifier();
  ~GaussianSupervisedClassifier();
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  GaussianSupervisedClassifier(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  typedef vnl_matrix<double> MatrixType; 
  typedef vnl_vector<double> VectorType;
  typedef vnl_matrix_fixed<double,1,VectorDimension> ColumnVectorType;

  MatrixType            m_Means;
  MatrixType            m_NumberOfSamples;
  MatrixType            *m_Covariance;  
  MatrixType            *m_InvCovariance;
  ColumnVectorType      m_TmpVec;
  ColumnVectorType      m_TmpMat;
  double                *m_PixelProbability;
  int                   m_ClassifiedPixelIndex;

  double                m_Epsilon;
  double                m_DoubleMax;
  bool                  m_ValidTrainingFlag;

}; // class GaussianSupervisedClassifier


} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGaussianSupervisedClassifier.txx"
#endif



#endif




