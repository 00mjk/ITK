/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLevenbergMarquardtOptimizer.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkLevenbergMarquardtOptimizer_txx
#define _itkLevenbergMarquardtOptimizer_txx

#include "itkLevenbergMarquardtOptimizer.h"

namespace itk
{

/**
 * Constructor
 */
template <class TCostFunction>
LevenbergMarquardtOptimizer<TCostFunction>
::LevenbergMarquardtOptimizer():
  m_LevenbergMarquardt( m_CostFunctionAdaptor )
{
}

/**
 * Get the Optimizer
 */
template <class TCostFunction>
LevenbergMarquardtOptimizer<TCostFunction>::InternalOptimizerType &
LevenbergMarquardtOptimizer<TCostFunction>
::GetOptimizer()
{
  return m_LevenbergMarquardt;
}



/**
 * Start the optimization
 */
template <class TCostFunction>
void
LevenbergMarquardtOptimizer<TCostFunction>
::StartOptimization( void )
{
  
  InternalParametersType initialParameters(SpaceDimension);
  
  VnlCostFunctionAdaptor::ConvertExternalToInternalParameters( 
                                GetInitialPosition()
                              , initialParameters );
  
  // vnl_levenberg_marquardt offers two methods for start minimization
  // depending on whether the cost_function knows how to compute gradients
  // or not
  // m_LevenbergMarquardt.minimize_using_gradient( initialParameters );
  m_LevenbergMarquardt.minimize( initialParameters );
  
}



} // end namespace itk

#endif
