/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkSimilarityIndexImageFilterTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkSimilarityIndexImageFilter.h"
#include "itkImageRegionIterator.h"

int itkSimilarityIndexImageFilterTest(int, char**)
{

  typedef unsigned char Pixel1Type;
  typedef float Pixel2Type;
  enum { ImageDimension = 2 };

  typedef itk::Image<Pixel1Type,ImageDimension> Image1Type;
  typedef itk::Image<Pixel2Type,ImageDimension> Image2Type;

  Image1Type::Pointer image1 = Image1Type::New();
  Image2Type::Pointer image2 = Image2Type::New();

  Image1Type::SizeType size;
  size.Fill( 8 );
  
  image1->SetRegions( size );
  image2->SetRegions( size );

  image1->Allocate();
  image2->Allocate();

  unsigned long numOfPixels = image1->GetBufferedRegion().GetNumberOfPixels();
  unsigned long lower1 = 0;
  unsigned long upper1 = (unsigned long) ( (double)numOfPixels * 0.75 ) - 1;
  unsigned long lower2 = (unsigned long) ( (double)numOfPixels * 0.25 );
  unsigned long upper2 = numOfPixels - 1;

  itk::ImageRegionIterator<Image1Type> it1( image1, image1->GetBufferedRegion() );
  itk::ImageRegionIterator<Image2Type> it2( image2, image2->GetBufferedRegion() );
  unsigned long count = 0;

  while( !it1.IsAtEnd() )
    {

    if ( lower1 <= count && count <= upper1 )
      {
      it1.Set( 5 );
      }
    else
      {
      it1.Set( 0 );
      }

    if ( lower2 <= count && count <= upper2 )
      {
      it2.Set( 7.2 );
      }
    else
      {
      it2.Set( 0 );
      }

    ++it1;
    ++it2;
    ++count;

    }

  typedef itk::SimilarityIndexImageFilter<Image1Type,Image2Type> FilterType;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput1( image1 );
  filter->SetInput2( image2 );
  
  filter->Update();

  filter->Print(std::cout);

  // check results
  FilterType::RealType trueOverlap = 0.5 / 0.75;
  FilterType::RealType overlap = filter->GetSimilarityIndex();

  std::cout << " True index: " << trueOverlap << std::endl;
  std::cout << " Computed index: " << overlap << std::endl;

  if ( vnl_math_abs( trueOverlap - overlap ) < 0.1 )
    {
    std::cout << "Test passed. " << std::endl;
    return EXIT_SUCCESS;
    }
  else
    {
    std::cout << "Test failed. " << std::endl;
    return EXIT_FAILURE;
    }
 
}
