/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLevelSetVelocityNeighborhoodExtractor.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkLevelSetVelocityNeighborhoodExtractor_txx
#define _itkLevelSetVelocityNeighborhoodExtractor_txx

#include "itkLevelSetVelocityNeighborhoodExtractor.h"
#include "vnl/vnl_math.h"

namespace itk
{


/*
 *
 */
template <class TLevelSet, class TAuxValue, 
unsigned int VAuxDimension>
LevelSetVelocityNeighborhoodExtractor<TLevelSet,TAuxValue,VAuxDimension>
::LevelSetVelocityNeighborhoodExtractor( )
{
  m_AuxInsideValues = 0;
  m_AuxOutsideValues = 0;
  for (unsigned int i=0; i < VAuxDimension; ++i)
    {
    m_AuxImage[i] = 0;
    }
}


/*
 *
 */
template <class TLevelSet, class TAuxValue,
  unsigned int VAuxDimension>
void
LevelSetVelocityNeighborhoodExtractor<TLevelSet,TAuxValue,VAuxDimension>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Input aux image: [";
  int j;
  for( j = 0; j < VAuxDimension - 1; j++ )
    {
    os << m_AuxImage[j].GetPointer() << ", ";
    }
  os << m_AuxImage[j].GetPointer() << "]";
}


/*
 *
 */
template <class TLevelSet, class TAuxValue,
  unsigned int VAuxDimension>
void
LevelSetVelocityNeighborhoodExtractor<TLevelSet,TAuxValue,VAuxDimension>
::Initialize()
{
  this->Superclass::Initialize();

  // create new empty auxiliary variable containers
  m_AuxInsideValues = AuxValueContainer::New();
  m_AuxOutsideValues = AuxValueContainer::New();

}


/*
 *
 */
template <class TLevelSet, class TAuxValue,
  unsigned int VAuxDimension>
double
LevelSetVelocityNeighborhoodExtractor<TLevelSet,TAuxValue,VAuxDimension>
::CalculateDistance(
Index& index)
{
  double distance = this->Superclass::CalculateDistance( index );
  if( distance >= this->GetLargeValue() )
    {
    return distance;
    }

  // is this an inside or outside point
  double pixelValue;
  PixelType inputPixel;

  inputPixel = (this->GetInputLevelSet())->GetPixel( index );
  pixelValue = (double) inputPixel;
  pixelValue -= this->GetLevelSetValue();

  bool inside = ( pixelValue <= 0.0 );
  double centerValue[VAuxDimension];
  AuxValueType auxPixel;
  AuxValueVectorType auxVector;

  for( unsigned int k = 0; k < VAuxDimension; k++ )
    {
    auxPixel = m_AuxImage[k]->GetPixel( index );
    centerValue[k] = (double) auxPixel;
    }

  // if distance is zero, insert point in inside container
  if( distance == 0.0 )
    {
    for( unsigned int k = 0; k < VAuxDimension; k++ )
      {
      auxVector[k] = centerValue[k];
      }

    m_AuxInsideValues->InsertElement( m_AuxInsideValues->Size(), auxVector );
    
    return distance;
    }

  double denom = 0.0;
  double numer[VAuxDimension];
  NodeType neighNode;

  for( unsigned int k = 0; k < VAuxDimension; k++ )
    {
    numer[k] = 0.0;
    }


 // The extend velcoity value is a weighted value of
 // the speed values at point used in the computation
 // of the distance by the superclass.
 //
 // The weights is proportional to one over the square
 // of distance along the grid line to the zero set 
 // crossing.

  for( int j = 0; j < SetDimension; j++ )
    {
    neighNode = this->GetNodeUsedInCalculation(j);
    if( neighNode.GetValue() >= this->GetLargeValue() )
      {
      break;
      }

    denom += 1.0 / vnl_math_sqr( neighNode.GetValue() );
    for( unsigned int k = 0; k < VAuxDimension; k++ )
      {
      auxPixel = m_AuxImage[k]->GetPixel( neighNode.GetIndex() );
      numer[k] += (double) ( auxPixel ) / vnl_math_sqr( neighNode.GetValue() );
      }

    }

  for( unsigned int k = 0; k < VAuxDimension; k++ )
    {
    numer[k] /= denom;
    auxVector[k] = numer[k];
    }

  if( inside )
    {
    m_AuxInsideValues->InsertElement( 
      m_AuxInsideValues->Size(), auxVector );
    }
  else
    {
    m_AuxOutsideValues->InsertElement( 
      m_AuxOutsideValues->Size(), auxVector );
    }

  return distance;

}

} // namespace itk

#endif
