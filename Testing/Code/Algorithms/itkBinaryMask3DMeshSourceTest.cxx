/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBinaryMask3DMeshSourceTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <math.h>
#include <iostream>
#include <time.h>

#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkMesh.h"
#include "itkBinaryMask3DMeshSource.h"


int itkBinaryMask3DMeshSourceTest(int, char *[])
{
  const int WIDTH  = 128;
  const int HEIGHT = 128;
  const int DEPTH  = 128;

  // Define the dimension of the images
  const unsigned int Dimension = 3;

  // Declare the types of the output images
  typedef itk::Image<unsigned short,   Dimension>   ImageType;

  // Declare the type of the index,size and region to initialize images
  typedef itk::Index<Dimension>                     IndexType;
  typedef itk::Size<Dimension>                      SizeType;
  typedef itk::ImageRegion<Dimension>               RegionType;
  typedef itk::ImageRegionIterator<ImageType>       IteratorType;
  typedef ImageType::PixelType                      PixelType;

  // Declare the type of the Mesh
  typedef itk::Mesh<double>                         MeshType;
  typedef MeshType::PointType                       PointType;

  typedef itk::BinaryMask3DMeshSource< MeshType >   MeshSourceType;

  int i, j, k;
  
  PixelType backgroundValue = 0;
  PixelType internalValue   = 1;
  
  SizeType size;
  size[0] = WIDTH;  
  size[1] = HEIGHT;  
  size[2] = DEPTH;  

  IndexType start;
  start.Fill(0);

  RegionType region;
  region.SetSize( size );
  region.SetIndex( start );
    
  ImageType::Pointer image = ImageType::New();

  image->SetRegions( region );
  image->Allocate();


  IteratorType it( image, region );
  it.GoToBegin();

  i = 0;
  j = 0;
  k = 0;
  while( !it.IsAtEnd() ) 
    {
    if ((i-64)*(i-64)+(j-64)*(j-64)+(k-64)*(k-64) < 200) it.Set( internalValue );
    else it.Set( backgroundValue );
    ++it;
    i++;
    if (i == WIDTH) 
      {
      i = 0;
      j++;
      if (j == HEIGHT) 
        {
        j = 0;
        k++;
        }
      }
    }
/*  
  SizeType smallerSize;
  smallerSize[0] = 100;
  smallerSize[1] = 100;
  smallerSize[2] = 100;

  IndexType internalStart;
  internalStart[0] = 50;
  internalStart[1] = 50;
  internalStart[2] = 50;

  // Create one iterator for an internal region
  RegionType internalRegion;
  internalRegion.SetSize( smallerSize );
  internalRegion.SetIndex( internalStart );

  IteratorType ir( image, internalRegion );
  ir.GoToBegin();
  while( !ir.IsAtEnd() )
    {
    ir.Set( internalValue );  
    ++ir;
    }
*/
  MeshSourceType::Pointer meshSource = MeshSourceType::New();

  meshSource->SetBinaryImage( image );
  meshSource->SetObjectValue( internalValue );

  try
    {
    meshSource->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown during Update() " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << meshSource->GetNumberOfNodes() << std::endl;
  std::cout << meshSource->GetNumberOfCells() << std::endl;

  return EXIT_SUCCESS;

}




