/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkTriangleCell.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkTriangleCell_h
#define __itkTriangleCell_h

#include "itkCellInterface.h"
#include "itkCellBoundary.h"
#include "itkLineCell.h"

namespace itk
{

/** \class TriangleCell
 * TriangleCell represents a triangle for a Mesh.
 *
 * The CellBoundary wrapper for this cell is TriangleBoundary.
 *
 * Template parameters for TriangleCell:
 *
 * TPixelType =
 *     The type associated with a point, cell, or boundary for use in storing
 *     its data.
 *
 * TCellTraits =
 *     Type information of mesh containing cell.
 *
 * \ingroup MeshObjects
 */

template < typename TCellInterface >
class TriangleCell: public TCellInterface
{
public:
  /** Standard class typedefs. */
  itkCellCommonTypedefs(TriangleCell);
  itkCellInheritedTypedefs(TCellInterface);

  /** Standard part of every itk Object. */
  itkTypeMacro(TriangleCell, CellInterface);
  
  /** The type of boundary for this triangle's vertices. */
  typedef VertexBoundary< TCellInterface >            VertexType;
  typedef typename VertexType::SelfAutoPointer        VertexAutoPointer;
  
  /** The type of boundary for this triangle's edges. */
  typedef LineBoundary< TCellInterface >              EdgeType;
  typedef typename EdgeType::SelfAutoPointer          EdgeAutoPointer;
    
  /** Triangle-specific topology numbers. */
  enum { NumberOfPoints   = 3,
         NumberOfVertices = 3,
         NumberOfEdges    = 3,
         CellDimension    = 2 };
  
  /** Implement the standard CellInterface. */
  virtual CellGeometry GetType(void) const 
    {return Superclass::TRIANGLE_CELL;}
  virtual void MakeCopy( CellAutoPointer & ) const;
  virtual unsigned int GetDimension(void) const;
  virtual unsigned int GetNumberOfPoints(void) const;
  virtual CellFeatureCount GetNumberOfBoundaryFeatures(int dimension) const;
  virtual bool GetBoundaryFeature(int dimension, CellFeatureIdentifier, CellAutoPointer &);
  virtual void SetPointIds(PointIdConstIterator first);
  virtual void SetPointIds(PointIdConstIterator first,
                           PointIdConstIterator last);
  virtual void SetPointId(int localId, PointIdentifier);
  virtual PointIdIterator      PointIdsBegin(void);
  virtual PointIdConstIterator PointIdsBegin(void) const;
  virtual PointIdIterator      PointIdsEnd(void);
  virtual PointIdConstIterator PointIdsEnd(void) const; 
  
  /** Triangle-specific interface. */
  virtual CellFeatureCount GetNumberOfVertices(void) const;
  virtual CellFeatureCount GetNumberOfEdges(void) const;
  virtual bool  GetVertex(CellFeatureIdentifier, VertexAutoPointer &);
  virtual bool  GetEdge(CellFeatureIdentifier, EdgeAutoPointer &);
  
  /** Cell visitor interface. */
  itkCellVisitMacro(TRIANGLE_CELL);

 public:
  TriangleCell() {}
  ~TriangleCell() {}

 protected:
  /** Store the number of points needed for a triangle. */
  PointIdentifier m_PointIds[NumberOfPoints];

  /** Triangle topology data. */
  static const int m_Edges[3][2];

 private:
  TriangleCell(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};


/** \class TriangleBoundary
 * Create a boundary-wrapped version of the TriangleCell.
 */
template <typename TCellInterface>
class TriangleBoundary:
  public CellBoundary< TriangleCell< TCellInterface > >
{
public:
  /** Standard class typedefs. */
  itkCellCommonTypedefs(TriangleBoundary);

  /** Standard part of every itk Object. */
  itkTypeMacro(TriangleBoundary, CellBoundary);

  /** Constructor and destructor */
  TriangleBoundary() {};
  ~TriangleBoundary() {};

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTriangleCell.txx"
#endif

#endif
