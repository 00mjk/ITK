/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFEMGenerateMesh.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkFEMGenerateMesh.h"
#include "itkFEMElement2DC0LinearQuadrilateral.h"
#include "itkFEMElement3DC0LinearHexahedron.h"
#include <math.h>
namespace itk {
namespace fem {



/*
 * Generate a rectangular mesh of quadrilateral elements
 */
template<>
void
GenerateMesh<Element2DC0LinearQuadrilateral>
::Rectangular(ElementType::ConstPointer e0, Solver& S, VectorType& orig, VectorType& size, VectorType& Nel)
{

  // Check for correct number of dimensions
  if(orig.size() != ElementType::NumberOfSpatialDimensions ||
     size.size() != ElementType::NumberOfSpatialDimensions ||
     Nel.size()  != ElementType::NumberOfSpatialDimensions)
  {
    throw FEMException(__FILE__, __LINE__, "GenerateMesh<Element2DC0LinearQuadrilateral>::Rectangular");
  }

  // Clear existing elements, loads and nodes in Solver
  S.load.clear();
  S.el.clear();
  S.node.clear();

  // Number of nodes in each dimension
  Nel[0]=floor(Nel[0]);
  Nel[1]=floor(Nel[1]);
  unsigned int Ni=static_cast<unsigned int>(Nel[0]);
  unsigned int Nj=static_cast<unsigned int>(Nel[1]);

  // Create nodes
  Node::Pointer n;
  int gn=0; // number of node
  for(unsigned int j=0; j<=Nj; j++)
  {
    for(unsigned int i=0; i<=Ni; i++)  
    {
      n=new Node(orig[0]+i*size[0]/Nel[0], orig[1]+j*size[1]/Nel[1]);
      n->GN=gn;
      gn++;
      S.node.push_back(FEMP<Node>(n));
    }
  }

  // Create elements  
  gn=0; // global number of the element
  ElementType::Pointer e;
  for(unsigned int j=0; j<Nj; j++)
  {
    for(unsigned int i=0; i<Ni; i++)
    {
      e=dynamic_cast<ElementType*>(e0->Clone());
      e->SetNode(0,S.node.Find( i+  (Ni+1)*j     ));
      e->SetNode(1,S.node.Find( i+1+(Ni+1)*j     ));
      e->SetNode(2,S.node.Find( i+1+(Ni+1)*(j+1) ));
      e->SetNode(3,S.node.Find( i+  (Ni+1)*(j+1) ));
      e->GN=gn;
      gn++;
      S.el.push_back(FEMP<Element>(e));
    }
  }

}



/*
 * Generate a rectangular mesh of hexahedron elements
 */
template<>
void
GenerateMesh<Element3DC0LinearHexahedron>
::Rectangular(ElementType::ConstPointer e0, Solver& S, VectorType& orig, VectorType& size, VectorType& Nel)
{

  // Check for correct number of dimensions
  if(orig.size() != ElementType::NumberOfSpatialDimensions ||
     size.size() != ElementType::NumberOfSpatialDimensions ||
     Nel.size()  != ElementType::NumberOfSpatialDimensions)
  {
    throw FEMException(__FILE__, __LINE__, "GenerateMesh<Element2DC0LinearQuadrilateral>::Rectangular");
  }

  // Number of nodes in each dimension
  Nel[0]=floor(Nel[0]);
  Nel[1]=floor(Nel[1]);
  Nel[2]=floor(Nel[2]);
  unsigned int Ni=static_cast<unsigned int>(Nel[0]);
  unsigned int Nj=static_cast<unsigned int>(Nel[1]);
  unsigned int Nk=static_cast<unsigned int>(Nel[2]);

  // Create nodes
  Node::Pointer n;
  int gn=0; // number of node
  for(unsigned int k=0; k<=Nk; k++)
  {
    for(unsigned int j=0; j<=Nj; j++)
    {
      for(unsigned int i=0; i<=Ni; i++)
      {
        n=new Node(orig[0]+i*size[0]/Nel[0],
                   orig[1]+j*size[1]/Nel[1],
                   orig[2]+k*size[2]/Nel[2]);
        n->GN=gn;
        gn++;
        S.node.push_back(FEMP<Node>(n));
      }
    }
  }

  // Create elements  
  gn=0; // global number of the element
  ElementType::Pointer e;
  for(unsigned int k=0; k<Nk; k++)
  {
    for(unsigned int j=0; j<Nj; j++)
    {
      for(unsigned int i=0; i<Ni; i++)
      {
        e=dynamic_cast<ElementType*>(e0->Clone());
        e->SetNode(0,S.node.Find( i+  (Ni+1)*(j  +(Nj+1)*k) ));
        e->SetNode(1,S.node.Find( i+1+(Ni+1)*(j  +(Nj+1)*k) ));
        e->SetNode(2,S.node.Find( i+1+(Ni+1)*(j+1+(Nj+1)*k) ));
        e->SetNode(3,S.node.Find( i+  (Ni+1)*(j+1+(Nj+1)*k) ));
        e->SetNode(4,S.node.Find( i+  (Ni+1)*(j  +(Nj+1)*(k+1)) ));
        e->SetNode(5,S.node.Find( i+1+(Ni+1)*(j  +(Nj+1)*(k+1)) ));
        e->SetNode(6,S.node.Find( i+1+(Ni+1)*(j+1+(Nj+1)*(k+1)) ));
        e->SetNode(7,S.node.Find( i+  (Ni+1)*(j+1+(Nj+1)*(k+1)) ));

        e->GN=gn;
        gn++;
        S.el.push_back(FEMP<Element>(e));
      }
    }
  }

}




}} // end namespace itk::fem
