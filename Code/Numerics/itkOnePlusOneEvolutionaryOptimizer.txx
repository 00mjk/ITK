/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    itkOnePlusOneEvolutionaryOptimizer.txx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) 2002 Insight Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __ONEPLUSONEEVOLUTIONARYOPTIMIZER_TXX
#define __ONEPLUSONEEVOLUTIONARYOPTIMIZER_TXX

#include "itkOnePlusOneEvolutionaryOptimizer.h"

namespace itk
{

template<class TNormalRandomVariateGenerator>
OnePlusOneEvolutionaryOptimizer<TNormalRandomVariateGenerator>
::OnePlusOneEvolutionaryOptimizer()
{
  m_Initialized = false ;
  m_Epsilon = (double) 1e-6  ; 
  m_RandomSeed = 0 ;
}

template<class TNormalRandomVariateGenerator>
OnePlusOneEvolutionaryOptimizer<TNormalRandomVariateGenerator>
::~OnePlusOneEvolutionaryOptimizer() 
{
}


template<class TNormalRandomVariateGenerator>
void 
OnePlusOneEvolutionaryOptimizer<TNormalRandomVariateGenerator>
::Initialize(double initialRadius, double grow, double shrink) 
{
  m_MaximumIteration = 20 ;
  m_InitialRadius = initialRadius ;

  if (grow == -1)
    m_GrowthFactor = 1.05 ;
  else
    m_GrowthFactor = grow ;

  if (shrink == -1)
    m_ShrinkFactor = pow(m_GrowthFactor, -0.25) ;
  else
    m_ShrinkFactor = shrink ;

  m_RandomGenerator = NormalRandomVariateGeneratorType::New() ;
  m_Initialized = true ;
}





template<class TNormalRandomVariateGenerator>
void
OnePlusOneEvolutionaryOptimizer<TNormalRandomVariateGenerator>
::Run()
  throw (ExceptionObject)
{
  
  if( m_CostFunction == 0 || !m_Initialized )
    {
      throw ExceptionObject(__FILE__, __LINE__);
    }

  // m_Random Seed was originally getpid()

  if (m_RandomSeed == 0)
    {
      m_RandomGenerator->Initialize((long) rand()); 
    }
  else
    {
      m_RandomGenerator->Initialize(m_RandomSeed); 
    }
  
  double pvalue, cvalue, adjust ;

  const unsigned int spaceDimension = m_CostFunction->GetNumberOfParameters();

  vnl_matrix<double> A(spaceDimension, spaceDimension, 0);
  vnl_vector<double> parent(this->GetInitialPosition()); 

  vnl_vector<double> f_norm(spaceDimension);
  vnl_vector<double> child(spaceDimension);
  vnl_vector<double> delta(spaceDimension);

  for(unsigned int i = 0  ; i < spaceDimension ; i++) 
    {
      A(i,i) = m_InitialRadius ;
    }
  //m_BiasField->SetCoefficients(parent) ;


  ParametersType parentPosition( spaceDimension );
  for( unsigned int i=0; i<spaceDimension; i++)
    {
      parentPosition[i] = parent[i];
    }

  pvalue = m_CostFunction->GetValue(parentPosition);
  this->SetCurrentPosition(parentPosition) ;

  for (unsigned int iter = 0 ; iter < m_MaximumIteration ; iter++) 
    {
      for (unsigned int i=0 ; i < spaceDimension ; i++) 
        {
          f_norm[i] = m_RandomGenerator->GetNormalVariate() ;
        }

      delta  = A * f_norm ;
      child  = parent + delta ;

      ParametersType childPosition( spaceDimension );
      for( unsigned int i=0; i<spaceDimension; i++)
        {
          childPosition[i] = child[i];
        }
      cvalue = m_CostFunction->GetValue(childPosition);

      itkDebugMacro(<< iter << ": parent: " << pvalue 
                    << " child: "<< cvalue );

      if (cvalue < pvalue) 
        {
          pvalue = cvalue ;
          
          parent.swap(child) ;                  
          
          adjust = m_GrowthFactor ; 
          for( unsigned int i=0; i<spaceDimension; i++)
            {
              parentPosition[i] = parent[i];
            }
          this->SetCurrentPosition(parentPosition) ;
          
        } 
      else 
        {
          adjust = m_ShrinkFactor ;
        }
      
      // convergence criterion: f-norm of A < epsilon_A
      // Compute double precision sum of absolute values of 
      // a single precision vector
      if ( this->GetDebug() )
        {
          if (A.fro_norm() <= m_Epsilon) 
            {
              itkDebugMacro(<< "A f-norm:" << A.fro_norm());
              break ;
            }
        }
      
      // A += (adjust - 1)/ (f_norm * f_norm) * A * f_norm * f_norm ;
      // Blas_R1_Update(A, A * f_norm, f_norm, 
      //             ((adjust - 1) / Blas_Dot_Prod(f_norm, f_norm)));    
      // = DGER(Fortran)
      //   performs the rank 1 operation
      // A := alpha*x*y' + A,
      // where y' = transpose(y)
      // where alpha is a scalar, x is an m element vector, y is an n element
      // vector and A is an m by n matrix.
      // x = A * f_norm , y = f_norm, alpha = (adjust - 1) / Blas_Dot_Prod(
      // f_norm, f_norm)
      A = A + (adjust - 1.0) * A ;
    }
}
 


/**
 *
 */

template<class TNormalRandomVariateGenerator>
void
OnePlusOneEvolutionaryOptimizer<TNormalRandomVariateGenerator>
::PrintSelf(std::ostream& os, Indent indent) const
{

  Superclass::PrintSelf(os,indent);

  os << indent << "Random Generator " << m_RandomGenerator  << std::endl;
  os << indent << "MaximumIteration " << m_MaximumIteration << std::endl;
  os << indent << "Epsilon          " << m_Epsilon          << std::endl;
  os << indent << "Initial Radius   " << m_InitialRadius    << std::endl;
  os << indent << "Growth Fractor   " << m_GrowthFactor     << std::endl;
  os << indent << "Shrink Fractor   " << m_ShrinkFactor     << std::endl;
  os << indent << "Random Seed      " << m_RandomSeed       << std::endl;
  os << indent << "Initialized      " << m_Initialized      << std::endl;
  os << indent << "Measure          " << m_Value            << std::endl;

}


} // end of namespace itk
#endif
