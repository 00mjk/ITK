/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkKalmanLinearEstimator.h
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
#ifndef __itkKalmanLinearEstimator_h
#define __itkKalmanLinearEstimator_h

#include "itkMacro.h"

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

namespace itk
{

/** \class KalmanLinearEstimator
 * \brief Implement a linear recursive estimator.
 *
 * KalmanLinearEstimator class implements a linear recursive estimator.  The
 * class is templated over the type of the parameters to be estimated and
 * over the number of parameters. Recursive estimation is a fast mechanism
 * for getting information about a system for which we only have access to
 * measures that are linearly related with the parameters we want to
 * estimate.
 *
 * \ingroup Numerics
 */
template <class T, unsigned int VEstimatorDimension>
class KalmanLinearEstimator 
{
public:
  /**  Dimension of the vector of parameters to be estimated.
   *  It is equivalent to the number of parameters to estimate. */  
  enum { Dimension = VEstimatorDimension };

  /**  Vector type defines a generic vector type that is used
   *  for the matricial operations performed during estimation. */
  typedef vnl_vector_fixed<T,VEstimatorDimension> Vector;

  /**  Matrix type defines a generic matrix type that is used
   *  for the matricial operations performed during estimation. */
  typedef vnl_matrix_fixed<T,VEstimatorDimension,VEstimatorDimension> Matrix;

  /** Type is the type associated with the parameters to be estimated.
   * All the parameters are of the same type. Natural choices could be
   * floats and doubles, because Type also is used for all the internal
   * computations. */
  typedef T Type;

  /** Update the estimation using the information provided by a new measure
   * along with a new line of the linear predictor. This method is the one
   * that should be called iteratively in order to estimate the parameter's
   * vector. It internally updates the covariance matrix. */
  void UpdateWithNewMeasure(  const T & newMeasure,
                              const Vector & newPredictor );

  /** This method resets the estimator. It set all the parameters to null.
   * The covariance matrix is not changed.
   * \sa Estimator \sa Variance \sa ClearVariance */
  void ClearEstimation(void) 
    { m_Estimator = 0; }

  /** This method resets the covariance matrix. It is set to an identity matrix
   * \sa Estimator \sa Variance \sa ClearEstimation */
  void ClearVariance(void)
    {
    m_Variance.set_identity();
    }

  /** This method sets the covariance matrix to a diagonal matrix with
   * equal values. It is useful when the variance of all the parameters
   * be estimated are the same and the parameters are considered independents.
   * \sa Estimator
   * \sa Variance
   * \sa ClearEstimation */
  void SetVariance(const T & var = 1.0) 
    {
    m_Variance.set_identity();
    m_Variance *= var;
    }

  /** This method sets the covariance matrix to known matrix. It is intended to
   * initialize the estimator with a priori information about the statistical
   * distribution of the parameters.  It can also be used to resume the
   * operation of a previously used estimator using it last known state.
   * \sa Estimator \sa Variance \sa ClearEstimation */
  void SetVariance(const Matrix & m)
    { m_Variance = m; }
  
  /** This method returns the vector of estimated parameters
   * \sa Estimator */ 
  const Vector& GetEstimator(void) const
    { return m_Estimator; }

  /** This method returns the covariance matrix of the estimated parameters
   * \sa Variance */
  const Matrix & GetVariance(void) const
    { return m_Variance; }

private:  
  /** This methods performs the update of the parameter's covariance matrix.
   * It is called by updateWithNewMeasure() method. Users are not expected to
   * call this method directly.
   * \sa updateWithNewMeasure */
  void UpdateVariance( const Vector & );

  /** Vector of parameters to estimate.
   * \sa GetEstimator */
  Vector m_Estimator;

  /** Estimation of the parameter's covariance matrix. This matrix contains
   * the information about the estate of the estimator. It holds all the
   * information obtained from previous measures provided to the
   * estimator. The initialization of this matrix is critical to the behavior
   * of the estimator, at least to ensure a short trasient period for
   * estabilizing the estimation.  \sa SetVariance \sa GetVariance */
  Matrix m_Variance;
};


template <class T, unsigned int VEstimatorDimension>
void
KalmanLinearEstimator<T,VEstimatorDimension>
::UpdateWithNewMeasure(const T & newMeasure, const Vector & newPredictor )
{
  T measurePrediction      = dot_product( newPredictor , m_Estimator );
  
  T errorMeasurePrediction = newMeasure - measurePrediction;

  Vector Corrector = m_Variance * newPredictor;

  for( unsigned int j=0; j<VEstimatorDimension; j++) 
    {
    m_Estimator(j) += Corrector(j) * errorMeasurePrediction;
    }
  
  UpdateVariance( newPredictor );
}


template <class T, unsigned int VEstimatorDimension>
void
KalmanLinearEstimator<T,VEstimatorDimension>
::UpdateVariance( const Vector & newPredictor )
{  
  Vector aux =  m_Variance * newPredictor;

  T denominator = 1.0/(1.0 +  dot_product( aux , newPredictor ) );

  unsigned pos = 0;
  for( unsigned int col=0; col<VEstimatorDimension; col++) 
    {
    for( unsigned int row=0; row<VEstimatorDimension; row++) 
      {
      m_Variance(col,row) -= aux(col)*aux(row)*denominator;
      pos++;
      }
    }
}

} // end namespace itk

#endif
