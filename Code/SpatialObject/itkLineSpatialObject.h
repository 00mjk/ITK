/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLineSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkLineSpatialObject_h
#define __itkLineSpatialObject_h

#include <list>

#include "itkSpatialObject.h"
#include "itkLineSpatialObjectPoint.h"

namespace itk 
{

/** 
* \class LineSpatialObject
* \brief Representation of a Line based on the spatial object classes.
*
* The Line is basically defined by a set of points. 
*
* \also LineSpatialObjectPoint 
*/

template < unsigned int TDimension = 3 , unsigned int PipelineDimension = 3 >
class LineSpatialObject 
:public SpatialObject<  TDimension, 
                        AffineTransform<double, TDimension>, 
                        PipelineDimension
                     >
{

public:

  typedef LineSpatialObject                           Self;
  typedef SpatialObject< TDimension, 
                         AffineTransform< double, TDimension>,
                         PipelineDimension
                         >                            Superclass;
  typedef SmartPointer < Self >                       Pointer;
  typedef SmartPointer < const Self >                 ConstPointer;
  typedef double                                      ScalarType;
  typedef LineSpatialObjectPoint< TDimension >        LinePointType;
  typedef typename LinePointType::Pointer             LinePointPointer; 
  typedef std::list < LinePointPointer >              PointListType;
  typedef typename Superclass::PointType              PointType;
  typedef VectorContainer<unsigned long,PointType>    PointContainerType;
  typedef SmartPointer<PointContainerType>            PointContainerPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Method for creation through the object factory. */
  itkTypeMacro( Self, Superclass );
  
  /** Returns a reference to the list of the Line points.*/
  PointListType & GetPoints( void );

  /** Set the list of line points. */
  void SetPoints( PointListType & newPoints );

  /** Returns true if the line is evaluable at the requested point, 
   *  false otherwise. */
  bool IsEvaluableAt( const PointType & point );

  /** Returns the value of the line at that point.
   * Currently this function returns a binary value,
   * but it might want to return a degree of membership
   * in case of fuzzy Lines. */
  void ValueAt( const PointType & point, double & value );

  /** Returns true if the point is inside the line, false otherwise. */
  bool IsInside( const PointType & point );

  /** Compute the boundaries of the line.*/
  void ComputeBounds( void );

  /** Return the last modified time of the object, and all of its components */
  unsigned long GetMTime( void ) const;

protected:

  PointListType   m_Points;
  TimeStamp       m_BoundsMTime; 

  LineSpatialObject();
  virtual ~LineSpatialObject();

  /** Method to print the object. */
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION 
  #include "itkLineSpatialObject.txx" 
#endif 

#endif // __itkLineSpatialObject_h
