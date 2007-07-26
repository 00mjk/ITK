// -------------------------------------------------------------------------
// itkQuadEdgeMeshEulerOperatorSplitEdgeFunction.h
// $Revision: 1.1 $
// $Author: hanfei $
// $Name:  $
// $Date: 2007-07-26 06:30:26 $
// -------------------------------------------------------------------------
// This code is an implementation of the well known quad edge (QE) data
// structure in the ITK library. Although the original QE can handle non
// orientable 2-manifolds and its dual and its mirror, this implementation
// is specifically dedicated to handle orientable 2-manifolds along with
// their dual.
//
// Any comment, criticism and/or donation is welcome.
//
// Please contact any member of the team:
//
// - The frog master (Eric Boix)       eboix@ens-lyon.fr
// - The duck master (Alex Gouaillard) alexandre.gouaillard@sun.com
// - The cow  master (Leonardo Florez) florez@creatis.insa-lyon.fr
// -------------------------------------------------------------------------
#ifndef __itkQuadEdgeMeshEulerOperatorSplitEdgeFunction_h
#define __itkQuadEdgeMeshEulerOperatorSplitEdgeFunction_h

#include "itkQuadEdgeMeshFunctionBase.h"
#include "itkQuadEdgeMeshEulerOperatorSplitVertexFunction.h"

namespace itk
{
/**
 * \class EulerOperatorSplitEdgeFunction
 * \ingroup QEMeshModifierFunctions
 *
 * \brief Given Edge is splitted into two and associated faces see their degree
 *        increased by one (a triangle is transformed into a quad for example).
 */
template < class TMesh, class TQEType >
class ITK_EXPORT QuadEdgeMeshEulerOperatorSplitEdgeFunction :
   public QuadEdgeMeshFunctionBase< TMesh, TQEType* >
{
public:
  /** Standard class typedefs. */
  typedef QuadEdgeMeshEulerOperatorSplitEdgeFunction      Self;
  typedef QuadEdgeMeshFunctionBase< TMesh, TQEType* >     Superclass;
  typedef itk::SmartPointer< Self >                       Pointer;
  typedef itk::SmartPointer< const Self >                 ConstPointer;

  itkNewMacro( Self );
  /** Run-time type information (and related methods). */
  itkTypeMacro( QuadEdgeMeshEulerOperatorSplitEdgeFunction, QuadEdgeMeshFunctionBase );

  /** Type of QuadEdge with which to apply slicing. */
  typedef TQEType QEType;

  typedef typename Superclass::MeshType      MeshType;
  typedef typename Superclass::OutputType    OutputType;
  typedef typename MeshType::PointIdentifier PointIdentifier;

  typedef QuadEdgeMeshEulerOperatorSplitVertexFunction< MeshType, QEType> SplitVertex;

  /** Evaluate at the specified input position */
  virtual OutputType Evaluate( QEType* e )
    {
    m_SplitVertex->SetInput( this->m_Mesh );
    return( m_SplitVertex->Evaluate( e->GetLprev( ), e->GetSym( ) ) );
    }
 
  const PointIdentifier GetNewPointID( )
    {
    return( m_SplitVertex->GetNewPointID( ) );
    }

protected:
  QuadEdgeMeshEulerOperatorSplitEdgeFunction()
    {
    m_SplitVertex = SplitVertex::New( );
    }
  ~QuadEdgeMeshEulerOperatorSplitEdgeFunction(){ };

private:
  QuadEdgeMeshEulerOperatorSplitEdgeFunction(const Self& ); //purposely not implemented
  void operator=(const Self& );        //purposely not implemented
  
  typename SplitVertex::Pointer m_SplitVertex; 

};

} // namespace itk

#endif

// eof - itkQuadEdgeMeshEulerOperatorSplitEdgeFunction.h
