/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit (ITK)
  Module:    itkMeshTest.cxx
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
#include "itkMesh.h"
#include "itkTetrahedronCell.h"
#include "itkHexahedronCell.h"
#include "itkBoundingBox.h"

#include <iostream>
#include <string>

/**
 * Some typedefs to make things easier.
 */

/**
 * Define a mesh type that stores a PixelType of "int".  Use the defaults
 * for the other template parameters.
 */
typedef itk::Mesh<int>  Mesh;
typedef Mesh::CellTraits  CellTraits;

/**
 * Define a few cell types which uses a PixelType of "int".  Again,
 * use the defaults for the other parameters.  Note that a cell's template
 * parameters must match those of the mesh into which it is inserted.
 */
typedef itk::LineBoundary<int, CellTraits>     LineBoundary;
typedef itk::TetrahedronCell<int, CellTraits>  TetraCell;
typedef itk::HexahedronCell<int, CellTraits>   HexaCell;

/**
 * Typedef the generic cell type for the mesh.  It is an abstract class,
 * so we can only use information from it, like get its pointer type.
 */
typedef Mesh::Cell  Cell;
typedef Cell        Boundary;

/**
 * The type of point stored in the mesh. Because mesh was instantiated
 * with defaults (itkDefaultStaticMeshTraits), the point dimension is 3 and
 * the coordinate representation is float.
 */
typedef Mesh::PointType  PointType;

/**
 * The mesh that is created consists of a single hexahedron and a single
 * tetrahedron. (The tetra is inside of the hex.)
 */



int main(void)
{
  /**
   * Define the 3d geometric positions for 8 points in a cube.
   */
  Mesh::CoordRepType testPointCoords[8][3]
    = { {0,0,0}, {9,0,0}, {9,0,9}, {0,0,9},
        {0,9,0}, {9,9,0}, {9,9,9}, {0,9,9} };
  
  /**
   * List the points that the tetrahedron will use from the mesh.
   */
  unsigned long tetraPoints[4] = {0,1,2,4};
  
  /**
   * List the points that the hexahedron will use from the mesh.
   */
  unsigned long hexaPoints[8] = {0,1,2,3,4,5,6,7};
  
  
  /**
   * Create the mesh through its object factory.
   */
  Mesh::Pointer mesh(Mesh::New());  

  /**
   * Add our test points to the mesh.
   * mesh->SetPoint(pointId, point)
   * Note that the constructor for Point is public, and takes an array
   * of coordinates for the point.
   */
  for(int i=0; i < 8 ; ++i)
    {
    mesh->SetPoint(i, PointType(testPointCoords[i]));
    }

  /**
   * Create the test cell. Note that testCell is a generic smart
   * pointer to a cell; in this example it ends up pointing to
   * different types of cells.
   */
  Cell::Pointer testCell(TetraCell::New());

  /**
   * Assign the points to the tetrahedron through their identifiers.
   */
  testCell->SetPointIds(tetraPoints);

  /**
   * Add the test cell to the mesh.
   * mesh->SetCell(cellId, cell)
   */
  mesh->SetCell(0, testCell);
  
  /**
   * Create another test cell.
   */
  testCell = HexaCell::New();
  testCell->SetPointIds(hexaPoints);
  mesh->SetCell(1, testCell);

  /**
   * Try getting one of the hexahedron's faces.
   */
  testCell = mesh->GetCellBoundaryFeature(
    2,    // Topological dimension of boundary.
    1,    // CellIdentifier.
    0);   // CellFeatureIdentifier

  std::cout << testCell->GetNameOfClass() << std::endl;
  
  /**
   * Allocate an explicity boundary line.
   */
  Boundary::Pointer boundLine(LineBoundary::New());
  
  /**
   * We don't want the hexahedron to consider the tetrahedron a neighbor
   * across its first edge, so don't add the tetrahedron as a using cell.
   */
  boundLine->AddUsingCell(1);
  boundLine->SetPointId(0,0);
  boundLine->SetPointId(1,1);
  
  mesh->SetBoundary(1,     // Topological dimension of boundary.
        0,       // Boundary identifier.
        boundLine);  // Pointer to explicit boundary.
  
  mesh->SetBoundaryAssignment(1,   // Topologoical dimension.
            1,   // CellIdentifier
            0,   // CellFeatureIdentifier
            0);  // Boundary identifier.  
  /**
   * Try getting the hexahedron's neighbor through its first edge.
   * This should be the test tetrahedron, except that we have done an
   * explicit assignment which removes this.
   */
  std::set<Mesh::CellIdentifier>  neighborSet;
  std::set<Mesh::CellIdentifier>::iterator cell;
  mesh->GetCellBoundaryFeatureNeighbors(
    1,              // Topological dimension of feature.
    1,              // CellIdentifier
    0,              // CellFeatureIdentifier
    &neighborSet); // Where to put result.

  std::cout << "Neighbors (hex edge 0):" << std::endl;
  for(cell = neighborSet.begin(); cell != neighborSet.end(); ++cell)
    {
    std::cout << "Id " << *cell << ": ";
    if(mesh->GetCell(*cell, &testCell))
      {
      std::cout << testCell->GetNameOfClass();
      }
    std::cout << std::endl;
    }
  
  /**
   * Try getting the hexahedron's neighbor through its second edge.
   * This should be the test tetrahedron. (Because the boundary is
   * not defined explicitly, we use implicit relationships to determine
   * neighbors. In this case, bit the tetrahedron and hexahedron share
   * the two points defining the edge and are therefore considered 
   * neighbors.)
   */
  mesh->GetCellBoundaryFeatureNeighbors(
    1,              // Topological dimension of feature.
    1,              // CellIdentifier
    1,              // CellFeatureIdentifier
    &neighborSet); // Where to put result.

  std::cout << "Neighbors (hex edge 1):" << std::endl;
  for(cell = neighborSet.begin(); cell != neighborSet.end(); ++cell)
    {
    std::cout << "Id " << *cell << ": ";
    if(mesh->GetCell(*cell, &testCell))
      {
      std::cout << testCell->GetNameOfClass();
      }
    std::cout << std::endl;
    }
  
  /**
   * Try getting the tetrahedrons's neighbor through its fourth edge.
   * This should be the test hexahedron. The boundaries are implicit
   * as in the previous example.
   */
  mesh->GetCellBoundaryFeatureNeighbors(
    1,              // Topological dimension of feature.
    0,              // CellIdentifier
    3,              // CellFeatureIdentifier
    &neighborSet); // Where to put result.

  std::cout << "Neighbors (tet edge 3):" << std::endl;
  for(cell = neighborSet.begin(); cell != neighborSet.end(); ++cell)
    {
    std::cout << "Id " << *cell << ": ";
    if(mesh->GetCell(*cell, &testCell))
      {
      std::cout << testCell->GetNameOfClass();
      }
    std::cout << std::endl;
    }
  

  /**
   * Perform some geometric operations (coordinate transformations)
   * to see if they are working.
   */
  Mesh::CoordRepType coords[Mesh::PointDimension];
  Mesh::PointIdentifier pointId;
  mesh->FindClosestPoint(coords,&pointId);

  /**
   * Compute the bounding box of the mesh
   */
  typedef itk::BoundingBox<Mesh::PointIdentifier,Mesh::PointDimension,
    Mesh::CoordRepType,Mesh::PointsContainer> BoundingBox;

  BoundingBox::Pointer bbox(BoundingBox::New());
  bbox->SetPoints(mesh->GetPoints());
  bbox->ComputeBoundingBox();
  std::cout << bbox << std::endl;

  mesh->Initialize();

  return 0;  
}

