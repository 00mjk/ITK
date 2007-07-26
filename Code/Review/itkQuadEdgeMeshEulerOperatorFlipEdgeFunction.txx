// -------------------------------------------------------------------------
// itkQuadEdgeMeshEulerOperatorFlipEdgeFunction.txx
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
#ifndef __itkQuadEdgeMeshEulerOperatorFlipEdgeFunction_txx
#define __itkQuadEdgeMeshEulerOperatorFlipEdgeFunction_txx

namespace itk
{

template < class TMesh, class TQEType >
typename QuadEdgeMeshEulerOperatorFlipEdgeFunction< TMesh, TQEType >::OutputType
QuadEdgeMeshEulerOperatorFlipEdgeFunction< TMesh, TQEType >::
Evaluate( QEType* h )
{
  //
  //    X ---<-G---- X              X ---<-G---- X
  //    |          / |              | \          |
  //    |         /  |              |  \         |
  //    |        /   |              |   \        |
  //    |       /    |              |    \       |
  //    |      ^     |              |     \      |
  //    |     h      |              |   newEdge  |
  //    |    /       |              |       \    |
  //    |   /        |              |        \   |
  //    |  /         |              |         \  |
  //    | /          |              |          \ |
  //    X ---H->---- X              X ---H->---- X
  //

  if( h == (QEType*)0 )
    {
    itkDebugMacro( "No Edge to flip." );
    return( (QEType*) 0 );
    }
   
  if( !this->m_Mesh )
    {
    itkDebugMacro( "No mesh present." );
    return( (QEType*) 0 );
    }
   
  if(  !h->IsInternal( ) )
    {
    itkDebugMacro( "Can only flip internal edge." );
    return( (QEType*) 0 );
    } 
     
  if( !h->IsLnextOfTriangle( ) || !h->GetSym( )->IsLnextOfTriangle( ) )
    {
    itkDebugMacro( "Can only flip edge for triangles." );
    return( (QEType*) 0 );
    }
   
  // The following is not optimum, since we create a new face (with JoinFacet)
  // that is immediately deleted (with SplitFacet). Still we chose to write it
  // that way in the sake of maintenance simplicity (as long as JoinFacet and
  // SplitFacet are working, this operator does it job).
  typedef itk::QuadEdgeMeshEulerOperatorJoinFacetFunction< MeshType, QEType > JoinFacet;
  typedef itk::QuadEdgeMeshEulerOperatorSplitFacetFunction< MeshType, QEType> SplitFacet;
   
  QEType* G = h->GetLnext( );
  typename JoinFacet::Pointer joinFacet = JoinFacet::New( );
  joinFacet->SetInput( this->m_Mesh );
  QEType* H = joinFacet->Evaluate( h )->GetLnext( );
   
  typename SplitFacet::Pointer splitFacet = SplitFacet::New( );
  splitFacet->SetInput( this->m_Mesh );
   
  return( splitFacet->Evaluate( H, G ) );
}

} // namespace itk

#endif

// eof - itkQuadEdgeMeshEulerOperatorFlipEdgeFunction.txx
