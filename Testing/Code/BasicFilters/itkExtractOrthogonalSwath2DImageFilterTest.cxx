/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkExtractOrthogonalSwath2DImageFilterTest.cxx
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

#include "itkPolyLineParametricPath.h"
#include "itkImage.h"
#include "itkExtractOrthogonalSwath2DImageFilter.h"

int itkExtractOrthogonalSwath2DImageFilterTest(int, char*[])
{
  typedef itk::Image<double, 2>                             ImageType; 
  typedef itk::PolyLineParametricPath<2>                    PathType;  

  typedef itk::ExtractOrthogonalSwath2DImageFilter<ImageType> FilterType;
  
  // Setup the inputs
  ImageType::Pointer  inImage = ImageType::New();
  PathType::Pointer   inPath  = PathType::New();
  
  // Setup the filter
  FilterType::Pointer filter = FilterType::New();
  filter->SetImageInput(inImage);
  filter->SetPathInput(inPath);
  
  // Setup the output
  ImageType::Pointer outImage;
  outImage=filter->GetOutput();
  
  // Test only compilation right now...
  return EXIT_SUCCESS;
}


