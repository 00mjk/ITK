/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkAmoebaOptimizer.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkAmoebaOptimizer_h
#define __itkAmoebaOptimizer_h

#include "itkSingleValuedNonLinearVnlOptimizer.h"
#include "vnl/algo/vnl_amoeba.h"

namespace itk
{
  
/** \class AmoebaOptimizer
 * \brief Wrap of the vnl_amoeba algorithm
 *
 * \ingroup Numerics Optimizers
 */
class ITK_EXPORT AmoebaOptimizer : 
    public SingleValuedNonLinearVnlOptimizer
{
public:
  /** Standard "Self" typedef. */
  typedef AmoebaOptimizer                     Self;
  typedef SingleValuedNonLinearVnlOptimizer   Superclass;
  typedef SmartPointer<Self>                  Pointer;
  typedef SmartPointer<const Self>            ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( AmoebaOptimizer, SingleValuedNonLinearVnlOptimizer );

  /** InternalParameters typedef. */
  typedef   vnl_vector<double>     InternalParametersType;

  /** Internal optimizer type. */
  typedef   vnl_amoeba             InternalOptimizerType;

  /** Method for getting access to the internal optimizer. */
  vnl_amoeba * GetOptimizer(void);

  /** Start optimization with an initial value. */
  void StartOptimization( void );

  /** Plug in a Cost Function into the optimizer  */
  virtual void SetCostFunction( SingleValuedCostFunction * costFunction );

protected:
  AmoebaOptimizer();
  virtual ~AmoebaOptimizer();

  typedef Superclass::CostFunctionAdaptorType   CostFunctionAdaptorType;

private:
  AmoebaOptimizer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  bool                          m_OptimizerInitialized;
  InternalOptimizerType       * m_VnlOptimizer;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAmoebaOptimizer.txx"
#endif

#endif



