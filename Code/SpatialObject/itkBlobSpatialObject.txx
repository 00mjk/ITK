/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBlobSpatialObject.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifndef __itkBlobSpatialObject_txx
#define __itkBlobSpatialObject_txx

#include "itkBlobSpatialObject.h" 

namespace itk  
{

/** Constructor */
template< unsigned int TDimension , unsigned int PipelineDimension >
BlobSpatialObject< TDimension, PipelineDimension > 
::BlobSpatialObject()  
{ 
  m_Dimension = TDimension;
  strcpy(m_TypeName,"BlobSpatialObject");
  m_Property->SetRed(1); 
  m_Property->SetGreen(0); 
  m_Property->SetBlue(0); 
  m_Property->SetAlpha(1); 
  m_Points = new PointListType();
  ComputeBounds();
} 

/** Destructor */ 
template< unsigned int TDimension , unsigned int PipelineDimension >
BlobSpatialObject< TDimension, PipelineDimension >  
::~BlobSpatialObject()
{ 
  delete m_Points;
} 
 
/** Get the list of points which are defining the blob */
template< unsigned int TDimension , unsigned int PipelineDimension >
BlobSpatialObject< TDimension, PipelineDimension > ::PointListPointer  
BlobSpatialObject< TDimension, PipelineDimension > 
::GetPoints() const 
{ 
  itkDebugMacro( "Getting BlobPoint list" );
  return m_Points;
} 

/** Set the points which are defining the Blob structure */
template< unsigned int TDimension , unsigned int PipelineDimension >
void  
BlobSpatialObject< TDimension, PipelineDimension >  
::SetPoints( PointListPointer points )  
{
  // in this function, passing a null pointer as argument will
  // just clear the list...
  itkDebugMacro( "Setting TubePoint list to " << points );
  m_Points->clear();
         
  if( points )
  {
    PointListType::iterator it,end;
    it = points->begin();    
    end = points->end();
    for(; it != end; it++ )
    {
      m_Points->push_back(*it);
    }
  }  
  this->Modified();
} 
 
/** Print the blob spatial object */
template< unsigned int TDimension , unsigned int PipelineDimension >
void  
BlobSpatialObject< TDimension, PipelineDimension >  
::PrintSelf( std::ostream& os, Indent indent ) const 
{ 
  os << indent << "BlobSpatialObject(" << this << ")" << std::endl; 
  os << indent << "ID: " << m_Id << std::endl; 
  os << indent << "nb of points: "<< m_Points->size() << std::endl;
  Superclass::PrintSelf( os, indent ); 
} 
  
/** Compute the bounds of the blob */ 
template< unsigned int TDimension , unsigned int PipelineDimension >
void 
BlobSpatialObject< TDimension, PipelineDimension >  
::ComputeBounds( void ) 
{ 
  itkDebugMacro( "Computing tube bounding box" );
  if( this->GetMTime() > m_BoundsMTime )
  {
    PointType pointLow, pointHigh; 
    PointType tempPointLow, tempPointHigh;
    PointListType::iterator it  = m_Points->begin();
    PointListType::iterator end = m_Points->end();

    PointContainerPointer points = PointContainerType::New();
    points->Initialize();

    for(unsigned int i=0; it!= end; it++, i++ ) 
    {     
      points->InsertElement(i,(*it)->GetPosition());
    } 

    m_Bounds->SetPoints(points);
    m_Bounds->ComputeBoundingBox();
    m_BoundsMTime.Modified();
  }
} 

/** Test if the given point is inside the blob
 *  Note: ComputeBounds should be called before. */
template< unsigned int TDimension , unsigned int PipelineDimension >
bool 
BlobSpatialObject< TDimension, PipelineDimension >  
::IsInside( const PointType & point )  
{
  itkDebugMacro( "Checking the point [" << point << "is inside the blob" );
  PointListType::iterator it = m_Points->begin();
    
  PointType transformedPoint = point;
  TransformPointToLocalCoordinate(transformedPoint);

  if( m_Bounds->IsInside(transformedPoint) )
  {
    while(it != m_Points->end())
    {
      if((*it)->GetPosition() == transformedPoint)
      {
        return true;
      }
      it++;
    }
  }
  return Superclass::IsInside(transformedPoint);
} 

/** Return true if the blob is evaluable at a given point 
 *  i.e if the point is defined in the points list        */
template< unsigned int TDimension , unsigned int PipelineDimension >
bool
BlobSpatialObject< TDimension, PipelineDimension > 
::IsEvaluableAt( const PointType & point )
{
   itkDebugMacro( "Checking if the tube is evaluable at " << point );
   return IsInside(point);
}


/** Return 1 if the point is in the points list */
template< unsigned int TDimension , unsigned int PipelineDimension >
void
BlobSpatialObject< TDimension, PipelineDimension > 
::ValueAt( const PointType & point, double & value )
{
  itkDebugMacro( "Getting the value of the tube at " << point );
  if( !IsEvaluableAt(point) )
  {
    value = 0;
    itk::ExceptionObject e("BlobSpatialObject.txx");
    e.SetLocation("BlobSpatialObject::ValueAt( const PointType & )");
    e.SetDescription("this object cannot provide a value at the requested point");
    throw e;
  }
  value = 1;
}

/** Get the modification time of the object */
template< unsigned int TDimension , unsigned int PipelineDimension >
unsigned long
BlobSpatialObject< TDimension, PipelineDimension > 
::GetMTime( void ) const
{
  unsigned long latestMTime = Object::GetMTime();
  unsigned long boundsMTime;

  if( (boundsMTime = m_Bounds->GetMTime()) > latestMTime )
  {
    latestMTime = boundsMTime;
  }
  return latestMTime;
}

} // end namespace itk 

#endif
