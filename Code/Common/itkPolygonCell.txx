/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkPolygonCell.txx
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
#ifndef _itkPolygonCell_txx
#define _itkPolygonCell_txx


namespace itk
{

/**
 * Standard CellInterface:
 */
template <typename TPixelType, typename TCellTraits>
PolygonCell< TPixelType , TCellTraits >::CellPointer
PolygonCell< TPixelType , TCellTraits >
::MakeCopy(void)
{

  CellPointer newCell(Self::New());
  newCell->SetPointIds(this->PointIdsBegin(),this->PointIdsEnd());
  return newCell;
}

  
/**
 * Standard CellInterface:
 * Get the topological dimension of this cell.
 */
template <typename TPixelType, typename TCellTraits>
int
PolygonCell< TPixelType , TCellTraits >
::GetDimension(void) const
{
  return Self::CellDimension;
}


/**
 * Standard CellInterface:
 * Get the number of points required to define the cell.
 */
template <typename TPixelType, typename TCellTraits>
int
PolygonCell< TPixelType , TCellTraits >
::GetNumberOfPoints(void) const
{
  return m_PointIds.size();
}  


/**
 * Standard CellInterface:
 * Get the number of boundary features of the given dimension.
 */
template <typename TPixelType, typename TCellTraits>
PolygonCell< TPixelType , TCellTraits >::CellFeatureCount
PolygonCell< TPixelType , TCellTraits >
::GetNumberOfBoundaryFeatures(int dimension) const
{
  switch (dimension)
    {
    case 0: return GetNumberOfVertices();
    case 1: return GetNumberOfEdges();
    default: return 0;
    }
}


/**
 * Standard CellInterface:
 * Get the boundary feature of the given dimension specified by the given
 * cell feature Id.
 * The Id can range from 0 to GetNumberOfBoundaryFeatures(dimension)-1.
 */
template <typename TPixelType, typename TCellTraits>
PolygonCell< TPixelType , TCellTraits >::CellPointer
PolygonCell< TPixelType , TCellTraits >
::GetBoundaryFeature(int dimension, CellFeatureIdentifier featureId)
{
  switch (dimension)
    {
    case 0: return CellPointer(GetVertex(featureId));
    case 1: return CellPointer(GetEdge(featureId));
    default: return CellPointer(NULL);
    }
}


/**
 * Standard CellInterface:
 * Set the point id list used by the cell.  It is assumed that the given
 * iterator can be incremented and safely de-referenced enough times to 
 * get all the point ids needed by the cell.
 */ 
template <typename TPixelType, typename TCellTraits>
void
PolygonCell< TPixelType , TCellTraits >
::SetPointIds(int dummy, int num, PointIdConstIterator first)
{
  PointIdConstIterator ii(first);
  m_PointIds.clear();
  for(int i=0; i < num ; ++i)
    {
    m_PointIds.push_back(*ii++);
    }
  m_NumberOfPoints = num;
  BuildEdges();
}

/**
 * after input the points in order, generate the edge connections
 */
template <typename TPixelType, typename TCellTraits>
void
PolygonCell< TPixelType , TCellTraits >
::BuildEdges(void){
  m_Edges.clear();
  if( m_NumberOfPoints > 0 )
    {
    m_Edges.resize(m_NumberOfPoints);
    for(int i = 1;i < m_NumberOfPoints; i++)
      {
      m_Edges[i][0]=i-1;
      m_Edges[i][1]=i;
      }
    m_Edges[m_NumberOfPoints-1][0]=m_NumberOfPoints-1;
    m_Edges[m_NumberOfPoints-1][1]=0;
    m_NumberOfEdges=m_NumberOfPoints;
    }
}

/**
 * Standard CellInterface:
 * Set the point id list used by the cell.  It is assumed that the given
 * iterator can be incremented and safely de-referenced enough times to 
 * get all the point ids needed by the cell.
 */ 
template <typename TPixelType, typename TCellTraits>
void
PolygonCell< TPixelType , TCellTraits >
::SetPointIds(PointIdConstIterator first)
{
}

/** 
 * Add one points to the points list
 */
template <typename TPixelType, typename TCellTraits>
void
PolygonCell< TPixelType , TCellTraits >
::AddPointId(PointIdentifier ptID)
{
  m_PointIds.push_back(ptID);
  m_NumberOfPoints++;
}

/**
 * clear all the point and edge informations
 */
template <typename TPixelType, typename TCellTraits>
void
PolygonCell< TPixelType , TCellTraits >
::ClearPoints(void)
{
  m_NumberOfPoints=0;
  m_NumberOfEdges=0;
  m_PointIds.clear();
  m_Edges.clear();
}

/**
 * Standard CellInterface:
 * Set the point id list used by the cell.  It is assumed that the range
 * of iterators [first, last) contains the correct number of points needed to
 * define the cell.  The position *last is NOT referenced, so it can safely
 * be one beyond the end of an array or other container.
 */ 
template <typename TPixelType, typename TCellTraits>
void
PolygonCell< TPixelType , TCellTraits >
::SetPointIds(PointIdConstIterator first, PointIdConstIterator last)
{
  m_PointIds.erase(m_PointIds.begin(), m_PointIds.end());
  m_PointIds.insert(m_PointIds.begin(), first, last);
  m_NumberOfPoints = m_PointIds.size();
  BuildEdges();
}

/**
 * Standard CellInterface:
 * Set an individual point identifier in the cell.
 */ 
template <typename TPixelType, typename TCellTraits>
void
PolygonCell< TPixelType , TCellTraits >
::SetPointId(int localId, PointIdentifier ptId)
{
  if(m_PointIds.size() < (unsigned int)(localId + 1)) {
    m_PointIds.resize( localId + 1 );
  }
  m_PointIds[localId] = ptId;
  m_NumberOfPoints = m_PointIds.size();
}


/**
 * Standard CellInterface:
 * Get a begin iterator to the list of point identifiers used by the cell.
 */
template <typename TPixelType, typename TCellTraits>
PolygonCell< TPixelType , TCellTraits >::PointIdIterator
PolygonCell< TPixelType , TCellTraits >
::PointIdsBegin(void)
{
  return &*(m_PointIds.begin());
}


/**
 * Standard CellInterface:
 * Get a const begin iterator to the list of point identifiers used
 * by the cell.
 */ 
template <typename TPixelType, typename TCellTraits>
PolygonCell< TPixelType , TCellTraits >::PointIdConstIterator
PolygonCell< TPixelType , TCellTraits >
::PointIdsBegin(void) const
{
  return &*(m_PointIds.begin());
}


/**
 * Standard CellInterface:
 * Get an end iterator to the list of point identifiers used by the cell.
 */ 
template <typename TPixelType, typename TCellTraits>
PolygonCell< TPixelType , TCellTraits >::PointIdIterator
PolygonCell< TPixelType , TCellTraits >
::PointIdsEnd(void)
{
  return &*(m_PointIds.end());
}


/**
 * Standard CellInterface:
 * Get a const end iterator to the list of point identifiers used
 * by the cell.
 */ 
template <typename TPixelType, typename TCellTraits>
PolygonCell< TPixelType , TCellTraits >::PointIdConstIterator
PolygonCell< TPixelType , TCellTraits >
::PointIdsEnd(void) const
{
  return &*(m_PointIds.end());
}


/**
 * Polygon-specific:
 * Get the number of vertices defining the Polygon.
 */
template <typename TPixelType, typename TCellTraits>
PolygonCell< TPixelType , TCellTraits >::CellFeatureCount
PolygonCell< TPixelType , TCellTraits >
::GetNumberOfVertices(void) const
{
  return m_PointIds.size();
}

/**
 * Polygon-specific:
 * Get the number of edges defined for the Polygon.
 */
template <typename TPixelType, typename TCellTraits>
PolygonCell< TPixelType , TCellTraits >::CellFeatureCount
PolygonCell< TPixelType , TCellTraits >
::GetNumberOfEdges(void) const
{
  return m_Edges.size();
}

/**
 * Polygon-specific:
 * Get the vertex specified by the given cell feature Id.
 * The Id can range from 0 to GetNumberOfVertices()-1.
 */ 
template <typename TPixelType, typename TCellTraits>
PolygonCell< TPixelType , TCellTraits >::VertexPointer
PolygonCell< TPixelType , TCellTraits >
::GetVertex(CellFeatureIdentifier vertexId)
{
  VertexPointer vert(Vertex::New());
  vert->SetPointId(0, m_PointIds[vertexId]);
  
  return vert;
}


/**
 * Polygon-specific:
 * Get the edge specified by the given cell feature Id.
 * The Id can range from 0 to GetNumberOfEdges()-1.
 */ 
template <typename TPixelType, typename TCellTraits>
PolygonCell< TPixelType , TCellTraits >::EdgePointer
PolygonCell< TPixelType , TCellTraits >
::GetEdge(CellFeatureIdentifier edgeId)
{
  EdgePointer edge(Edge::New());
  unsigned int max_pointId = GetNumberOfPoints() - 1;

  if( edgeId < max_pointId ){
    edge->SetPointId(0, m_PointIds[edgeId]);
    edge->SetPointId(1, m_PointIds[edgeId+1]);
  }
  else if( edgeId == max_pointId ) {
    edge->SetPointId(0, m_PointIds[max_pointId] );
    edge->SetPointId(1, m_PointIds[0] );
  }
  return edge;
}


} // end namespace itk

#endif
