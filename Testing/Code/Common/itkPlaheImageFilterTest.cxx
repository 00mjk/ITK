#include <iostream>

#include "itkReadMetaImage.h" 
#include "itkWriteMetaImage.h"

#include "itkPlaheImageFilter.h"
#include "itkImage.h"
#define dimension 2

int main()
{
  
  typedef itk::Image<unsigned short, dimension>           ImageType;
  typedef itk::PlaheImageFilter<unsigned short,dimension> PlaheType;
  
  PlaheType::Pointer plahe = PlaheType::New(); 

  // Create a source image 
  ImageType::RegionType region;
  ImageType::IndexType index;
  ImageType::SizeType size;
  for (int i = 0; i < dimension; i++)
    {
    index[i] = 0;
    size[i]  = 100;
    }
  region.SetIndex(index);
  region.SetSize(size);

  ImageType::Pointer image = ImageType::New();
  image->SetBufferedRegion( region );
  image->SetRequestedRegion( region );
  image->SetLargestPossibleRegion( region );
  image->Allocate();

  plahe->SetInput( image );

  // window[] is a neighborhood of a evaluated pixel
  unsigned int window[dimension];
  window[0] = 9;
  window[1] = 9;

  // Set window, alpha, and beta which are parameters for Plahe algorithm
  std::cout << "Set Plahe parameters" << endl;
  plahe->SetWindow(window);
  plahe->SetAlpha(0.3);
  plahe->SetBeta(0.3);
 
  // execute plahe algorithm
  std::cout << "Run Plahe Algorithm" << endl;
  plahe->GenerateData();

 
  return 0;
}

