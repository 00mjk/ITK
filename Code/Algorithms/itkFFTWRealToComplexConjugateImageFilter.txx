/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFFTWRealToComplexConjugateImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkFFTWRealToComplexConjugateImageFilter_txx
#define _itkFFTWRealToComplexConjugateImageFilter_txx
#include "itkFFTWRealToComplexConjugateImageFilter.h"
#include <iostream>
#include "itkIndent.h"
#include "itkMetaDataObject.h"

namespace itk
{

template <class TPixel, unsigned int Dimension>
void
FFTWRealToComplexConjugateImageFilter<TPixel,Dimension>::
GenerateData()
{
  // get pointers to the input and output
  typename TInputImageType::ConstPointer  inputPtr  = this->GetInput();
  typename TOutputImageType::Pointer      outputPtr = this->GetOutput();

  if ( !inputPtr || !outputPtr )
    {
    return;
    }
  
  const typename TInputImageType::SizeType&   inputSize
    = inputPtr->GetLargestPossibleRegion().GetSize();
  unsigned int num_dims = inputPtr->GetImageDimension();

  if(num_dims != outputPtr->GetImageDimension())
    return;

  // allocate output buffer memory
  outputPtr->SetBufferedRegion( outputPtr->GetRequestedRegion() );
  outputPtr->Allocate();

  std::complex<TPixel> *out = outputPtr->GetBufferPointer();
  if(typeid(TPixel) == typeid(double))
    {
    double *in = const_cast<double *>(reinterpret_cast<const double *>(inputPtr->GetBufferPointer()));
    fftw_complex *dptr = reinterpret_cast<double (*)[2]>(out);
    switch(num_dims)
      {
      case 1:
        M_plan = fftw_plan_dft_r2c_1d(inputSize[0],
                                      in,dptr,
                                      FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        break;
      case 2:
        M_plan = fftw_plan_dft_r2c_2d(inputSize[0],inputSize[1],
                                      in,dptr,
                                      FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        break;
      case 3:
        M_plan = fftw_plan_dft_r2c_3d(inputSize[0],inputSize[1],inputSize[2],
                                      in,dptr,
                                      FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        break;
      default:
        int *sizes = new int[num_dims];
        for(unsigned int i = 0; i < num_dims; i++)
          {
          sizes[(num_dims - 1) - i] = inputSize[i];
          }
      
        M_plan = fftw_plan_dft_r2c(num_dims,sizes,
                                   in,dptr,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        delete []sizes;
      }
    M_PlanComputed = true;
    fftw_execute(M_plan);
    }
  else if(typeid(TPixel) == typeid(float))
    {
    float *in = 
      const_cast<float *>(reinterpret_cast<const float *>(inputPtr->GetBufferPointer()));
    fftwf_complex *dptr = reinterpret_cast<float (*)[2]>(out);
    switch(num_dims)
      {
      case 1:
        M_planf = fftwf_plan_dft_r2c_1d(inputSize[0],
                                      in,dptr,
                                      FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        break;
      case 2:
        M_planf = fftwf_plan_dft_r2c_2d(inputSize[0],inputSize[1],
                                      in,dptr,
                                      FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        break;
      case 3:
        M_planf = fftwf_plan_dft_r2c_3d(inputSize[0],inputSize[1],inputSize[2],
                                      in,dptr,
                                      FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        break;
      default:
        int *sizes = new int[num_dims];
        for(unsigned int i = 0; i < num_dims; i++)
          {
          sizes[(num_dims - 1) - i] = inputSize[i];
          }
      
        M_planf = fftwf_plan_dft_r2c(num_dims,sizes,
                                   in,dptr,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        delete []sizes;
      }
    M_PlanComputedf = true;
    fftwf_execute(M_planf);
    }
}

template <class TPixel, unsigned int Dimension>
bool
FFTWRealToComplexConjugateImageFilter<TPixel,Dimension>::
FullMatrix()
{
  return false;
}

template <class TPixel, unsigned int Dimension>
void
FFTWRealToComplexConjugateImageFilter<TPixel,Dimension>::
PrintSelf(std::ostream& os,Indent indent) const
{
}
}


#endif
