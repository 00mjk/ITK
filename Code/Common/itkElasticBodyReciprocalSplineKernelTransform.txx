/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkElasticBodyReciprocalSplineKernelTransform.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkElasticBodyReciprocalSplineKernelTransform_txx
#define _itkElasticBodyReciprocalSplineKernelTransform_txx
#include "itkElasticBodyReciprocalSplineKernelTransform.h"

namespace itk
{

template <class TScalarType, unsigned int NDimensions>
ElasticBodyReciprocalSplineKernelTransform<TScalarType, NDimensions>::
ElasticBodyReciprocalSplineKernelTransform() 
{
  m_Alpha = .25;
}

template <class TScalarType, unsigned int NDimensions>
ElasticBodyReciprocalSplineKernelTransform<TScalarType, NDimensions>::
~ElasticBodyReciprocalSplineKernelTransform()
{
}

template <class TScalarType, unsigned int NDimensions>
const ElasticBodyReciprocalSplineKernelTransform<TScalarType, NDimensions>::GMatrixType &
ElasticBodyReciprocalSplineKernelTransform<TScalarType, NDimensions>
::ComputeG(const InputVectorType & x) const
{
  const TScalarType r       = x.GetNorm();
  const TScalarType factor  = 
         ( r > 1e-8 ) ? ( -1.0 / r ): NumericTraits<TScalarType>::Zero;
  const TScalarType radial  = m_Alpha * r;
  for(unsigned int i=0; i<NDimensions; i++)
    {
    const typename InputVectorType::ValueType xi = x[i] * factor;
    // G is symmetric
    for(unsigned int j=0; j<i; j++)
      {
        const TScalarType value = xi * x[j]; 
        m_GMatrix[i][j] = value;
        m_GMatrix[j][i] = value;
      }
    m_GMatrix[i][i] =  radial + xi * x[i];
    }
  
  return m_GMatrix;
}



} // namespace itk
#endif
