#include "itkIsotropicFourthOrderLevelSetImageFilter.h"
#include "itkImage.h"
#include <iostream>

const unsigned int HEIGHT = (128);
const unsigned int WIDTH  = (128);
const int LOW    = (34);
const int HIGH   = (94);

int itkIsotropicFourthOrderLevelSetImageFilterTest(int, char* [] )
{
  typedef itk::Image<float, 2> ImageType;
  typedef ImageType::IndexType IndexType;
  
  ImageType::Pointer im_init = ImageType::New();
  
  ImageType::RegionType r;
  ImageType::SizeType   sz = {{HEIGHT, WIDTH}};
  ImageType::IndexType  idx = {{0,0}};
  r.SetSize(sz);
  r.SetIndex(idx);

  im_init->SetLargestPossibleRegion(r);
  im_init->SetBufferedRegion(r);
  im_init->SetRequestedRegion(r);
  im_init->Allocate();

  IndexType index;

  for ( index[0]=0; index[0]<WIDTH; index[0]++ )
    for ( index[1]=0; index[1]<HEIGHT; index[1]++ )
      {
      if ( (index[0]>=LOW) && (index[0]<=HIGH) &&
           (index[1]>=LOW) && (index[1]<=HIGH) )
        {
        im_init->SetPixel (index, static_cast<float>(-1));
        
        }
      else
        {
        im_init->SetPixel (index, static_cast<float>(1));
        }
      }
  
  typedef itk::IsotropicFourthOrderLevelSetImageFilter<ImageType,
    ImageType> FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetMaxFilterIteration (250);
    
  filter->SetInput(im_init);
  std::cout<<"max iteration = "<<(filter->GetMaxFilterIteration())<<"\n";
  std::cout<<"Starting processing.\n";
  filter->Update();
  std::cout<<"Passed.\n";
  return 0;
}
