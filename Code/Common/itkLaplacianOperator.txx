/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLaplacianOperator.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkLaplacianOperator_txx
#define _itkLaplacianOperator_txx
#include "itkLaplacianOperator.h"

namespace itk
{

template<class TPixel, unsigned int VDimension, class TAllocator>
void
LaplacianOperator<TPixel, VDimension, TAllocator>
::SetDerivativeScalings( const double *s )
{
  for (unsigned int i = 0; i < VDimension; ++i)
    {
    m_DerivativeScalings[i] = s[i];
    }
}


//Create the operator
template <class TPixel, unsigned int VDimension, class TAllocator>
void
LaplacianOperator<TPixel, VDimension, TAllocator>
::CreateOperator()
{
  CoefficientVector coefficients;
  
  coefficients = this->GenerateCoefficients();

  this->Fill(coefficients);
  
}

//This function fills the coefficients into the corresponding neighborhodd.
template <class TPixel, unsigned int VDimension, class TAllocator>
void  
LaplacianOperator <TPixel, VDimension, TAllocator>
::Fill(const CoefficientVector &coeff)
{

  Superclass::CoefficientVector::const_iterator it;

  std::slice* temp_slice;
  temp_slice = new std::slice(0, coeff.size(),1);
  
  typename Self::SliceIteratorType data(this, *temp_slice);
  delete temp_slice;
 
  it = coeff.begin();

  // Copy the coefficients into the neighborhood
  for (data = data.Begin(); data < data.End(); ++data, ++it)
    {
    *data = *it;
    }

}

template <class TPixel, unsigned int VDimension, class TAllocator>
typename LaplacianOperator<TPixel, VDimension, TAllocator>
::CoefficientVector
LaplacianOperator<TPixel, VDimension, TAllocator>
::GenerateCoefficients()
{
  unsigned int i, w;
  
  // Create a vector of the correct size to hold the coefficients.
  w = 1;
  for(i = 0; i < VDimension; i ++)
    {
    w = w*3;
    }

  std::vector<PixelType> coeff(w);
  CoefficientVector coeffP(w);
  int offset[2 * VDimension];

  // Initialize coeff to 0.0
  for (i = 0; i < w; i++)
    {
    coeff[i] = 0.0;
    }
  
  // Here we set the radius to 1's, here the
  // operator is 3x3 for 2D, 3x3x3 for 3D.

  unsigned long k[VDimension];

  for (i = 0; i<VDimension; ++i)
    {
    k[i] = 1;
    }

  this->SetRadius(k);
  
  for ( i = 0 ; i < 2 * VDimension; i+= 2)
    {
    offset[i]   = - static_cast<long>( this->GetStride(i/2) );
    offset[i+1] =   static_cast<long>( this->GetStride(i/2) );
    }
  
  //Set the coefficients 
  for (i = 0 ; i < 2 * VDimension; i++)
    {
    coeff[w/2 + offset[i]] = 1.0 / m_DerivativeScalings[ i / 2];
    } 

  double sum = 0.0;
  for ( i = 0; i < w; i ++)
    {
    coeffP[i] = coeff[i] / (2.0 * static_cast<double>(VDimension) ) ;
    sum += coeffP[i];
    }
  
  coeffP[w/2] = - sum;
  
  return coeffP;
}

} // namespace itk

#endif
