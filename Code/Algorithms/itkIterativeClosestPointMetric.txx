/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkIterativeClosestPointMetric.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkIterativeClosestPointMetric_txx
#define _itkIterativeClosestPointMetric_txx

#include "itkIterativeClosestPointMetric.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{

/** Constructor */
template <class TFixedPointSet, class TMovingPointSet, class TDistanceMap> 
IterativeClosestPointMetric<TFixedPointSet,TMovingPointSet,TDistanceMap>
::IterativeClosestPointMetric()
{
  m_DistanceMap = 0;
}

/** Return the number of values, i.e the number of points in the moving set */
template <class TFixedPointSet, class TMovingPointSet, class TDistanceMap>  
unsigned int
IterativeClosestPointMetric<TFixedPointSet,TMovingPointSet,TDistanceMap>  
::GetNumberOfValues() const
{
 MovingPointSetConstPointer movingPointSet = this->GetMovingPointSet();

 if( !movingPointSet ) 
    {
    itkExceptionMacro( << "Moving point set has not been assigned" );
    }

 return  movingPointSet->GetPoints()->Size();
}


/** Get the match Measure */
template <class TFixedPointSet, class TMovingPointSet, class TDistanceMap>  
typename IterativeClosestPointMetric<TFixedPointSet,TMovingPointSet,TDistanceMap>::MeasureType
IterativeClosestPointMetric<TFixedPointSet,TMovingPointSet,TDistanceMap>
::GetValue( const TransformParametersType & parameters ) const
{
  FixedPointSetConstPointer fixedPointSet = this->GetFixedPointSet();

  if( !fixedPointSet ) 
    {
    itkExceptionMacro( << "Fixed point set has not been assigned" );
    }

  
  MovingPointSetConstPointer movingPointSet = this->GetMovingPointSet();

  if( !movingPointSet ) 
    {
    itkExceptionMacro( << "Moving point set has not been assigned" );
    }

  PointIterator pointItr = movingPointSet->GetPoints()->Begin();
  PointIterator pointEnd = movingPointSet->GetPoints()->End();


  MeasureType measure;
  measure.resize(movingPointSet->GetPoints()->Size());

  this->SetTransformParameters( parameters );

  unsigned int id = 0;
  while( pointItr != pointEnd )
    {
    typename Superclass::InputPointType  inputPoint;
    inputPoint.CastFrom( pointItr.Value() );
    typename Superclass::OutputPointType transformedPoint = 
      m_Transform->TransformPoint( inputPoint );

    double minimumDistance = NumericTraits<double>::max();
    bool closestPoint = false;

    // Try to use the distance map to solve the closest point
    if(m_DistanceMap)
      {
      // If the point is inside the distance map
      typename DistanceMapType::IndexType index;
      if(m_DistanceMap->TransformPhysicalPointToIndex(transformedPoint,index))
        {
        minimumDistance = m_DistanceMap->GetPixel(index);
        closestPoint = true;
        }
      }

    // if the closestPoint has not been found
    if(!closestPoint)
      {
      // Go trough the list of fixed point and find the closest distance
      PointIterator pointItr2 = fixedPointSet->GetPoints()->Begin();
      PointIterator pointEnd2 = fixedPointSet->GetPoints()->End();

      while( pointItr2 != pointEnd2 )
        {

        double dist = 0;
        for(unsigned int i=0;i<itkGetStaticConstMacro(MovingPointSetDimension);i++)
          {
          dist += (pointItr2.Value()[i] - transformedPoint[i])*(pointItr2.Value()[i] - transformedPoint[i]);
          }

        if(dist<minimumDistance)
          {
          minimumDistance = dist;
          }
        pointItr2++;
        }
      }

    measure.put(id,minimumDistance);

    ++pointItr;
    id++;
    }

  return measure;

}

/** Get the Derivative Measure */
template <class TFixedPointSet, class TMovingPointSet, class TDistanceMap>
void
IterativeClosestPointMetric<TFixedPointSet,TMovingPointSet,TDistanceMap>
::GetDerivative( const TransformParametersType & parameters,
                 DerivativeType & derivative ) const
{

}

/** Get both the match Measure and theDerivative Measure  */
template <class TFixedPointSet, class TMovingPointSet, class TDistanceMap>  
void
IterativeClosestPointMetric<TFixedPointSet,TMovingPointSet,TDistanceMap>
::GetValueAndDerivative(const TransformParametersType & parameters, 
                        MeasureType & value, DerivativeType  & derivative) const
{
  value = this->GetValue(parameters);
  this->GetDerivative(parameters,derivative);
}

/** PrintSelf method */
template <class TFixedPointSet, class TMovingPointSet, class TDistanceMap>  
void
IterativeClosestPointMetric<TFixedPointSet,TMovingPointSet,TDistanceMap>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "DistanceMap: " << m_DistanceMap << std::endl;
}

} // end namespace itk


#endif
