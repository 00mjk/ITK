/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkZeroCrossingImageFilter.txx
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

#ifndef _itkZeroCrossingImageFilter_txx
#define _itkZeroCrossingImageFilter_txx


#include "itkConstNeighborhoodIterator.h"
#include "itkConstSmartNeighborhoodIterator.h"
#include "itkZeroCrossingImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkArray.h"

namespace itk
{



template <class TInputImage, class TOutputImage>
void 
ZeroCrossingImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion() throw(InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // get pointers to the input and output
  InputImagePointer  inputPtr = this->GetInput();
  OutputImagePointer outputPtr = this->GetOutput();
  
  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  // Build an operator so that we can determine the kernel size

  unsigned long radius = 1;
  
  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion;
  inputRequestedRegion = inputPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius( radius );

  // crop the input requested region at the input's largest possible region
  if ( inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()) )
    {
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    return;
    }
  else
    {
    // Couldn't crop the region (requested region is outside the largest
    // possible region).  Throw an exception.

    // store what we tried to request (prior to trying to crop)
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    
    // build an exception
    InvalidRequestedRegionError e(__FILE__, __LINE__);
    std::ostrstream msg;
    msg << (char *)this->GetNameOfClass()
        << "::GenerateInputRequestedRegion()" << std::ends;
    e.SetLocation(msg.str());
    e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
    e.SetDataObject(inputPtr);
    throw e;
    }
}


template< class TInputImage, class TOutputImage >
void
ZeroCrossingImageFilter< TInputImage, TOutputImage >
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{

  unsigned int i;
  ZeroFluxNeumannBoundaryCondition<TInputImage> nbc;

  ConstNeighborhoodIterator<TInputImage> nit;
  ConstSmartNeighborhoodIterator<TInputImage> bit;
  ImageRegionIterator<TOutputImage> it;

  // Allocate output
  typename OutputImageType::Pointer output = this->GetOutput();
  typename  InputImageType::Pointer input  = this->GetInput();
  

  // Calculate iterator radius
  Size<ImageDimension> radius;
  for (i = 0; i < ImageDimension; ++i) radius[i]  = 1;

  // Find the data-set boundary "faces"
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TInputImage>::
    FaceListType faceList;
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TInputImage> bC;
  faceList = bC(input, outputRegionForThread, radius);

  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TInputImage>::
    FaceListType::iterator fit;
  fit = faceList.begin();

  // support progress methods/callbacks
  unsigned long ii = 0;
  unsigned long updateVisits = 0;
  unsigned long totalPixels = 0;
  if ( threadId == 0 )
    {
    totalPixels = outputRegionForThread.GetNumberOfPixels();
    updateVisits = totalPixels / 10;
    if( updateVisits < 1 ) updateVisits = 1;
    }

  // Process non-boundary face
  nit = ConstNeighborhoodIterator<TInputImage>(radius, input, *fit);
  it  = ImageRegionIterator<TOutputImage>(output, *fit);

  nit.GoToBegin();
  it.GoToBegin();


  InputImagePixelType this_one, that, abs_this_one, abs_that;
  InputImagePixelType zero = NumericTraits<InputImagePixelType>::Zero;
  OutputImagePixelType one = NumericTraits<OutputImagePixelType>::One;

  unsigned long center;
  Array<unsigned long, 2 * ImageDimension> offset;

  //set offset of neighbors to the center pixel
  for ( i = 0 ; i < 2 * ImageDimension; i+= 2)
    {
      offset[i] = - nit.GetStride(i/2);
      offset[i+1] = nit.GetStride(i/2);
    }

  // Now Process the non-boundary region.
  while( ! nit.IsAtEnd() )
    {
      if ( threadId == 0 && !(ii % updateVisits ) )
        {
          this->UpdateProgress((float)ii++ / (float)totalPixels);
        }
      
      center = nit.Size()/2;
      this_one = nit.GetPixel(center);
      
      for( i = 0; i< ImageDimension * 2; i++)
        {
          
          that = nit.GetPixel(center + offset[i]);

          if( ((this_one < zero) && (that > zero))
              || ((this_one > zero) && (that < zero)) 
              || ((this_one == zero) && (that != zero))
            || ((this_one != zero) && (that == zero))  )
            {
              abs_this_one =  ::abs(this_one);
              abs_that = ::abs(that);
              if(abs_this_one < abs_that)
                {
                  it.Value() = one;
                break;
                }
            }
        }

      ++nit;
      ++it;
    }
  
  // Process each of the boundary faces.  These are N-d regions which border
  // the edge of the buffer.
  for (++fit; fit != faceList.end(); ++fit)
    { 
    if ( threadId == 0 && !(ii % updateVisits ) )
      {
      this->UpdateProgress((float)ii++ / (float)totalPixels);
      }

    bit = ConstSmartNeighborhoodIterator<InputImageType>(radius,
                                                         input, *fit);
    it = ImageRegionIterator<OutputImageType>(output, *fit);
    bit.OverrideBoundaryCondition(&nbc);
    bit.GoToBegin();
    
    while ( ! bit.IsAtEnd() )
      {

        center = bit.Size()/2;
        this_one = nit.GetPixel(center);
        
        for( i = 0; i< ImageDimension * 2; i++)
          {
        
            that = nit.GetPixel(center + offset[i]);
            if( ((this_one < zero) && (that > zero))
                || ((this_one > zero) && (that < zero)) 
                || ((this_one == zero) && (that != zero))
                || ((this_one != zero) && (that == zero))  )
              {
                abs_this_one =  ::abs(this_one);
                abs_that = ::abs(that);
                if(abs_this_one < abs_that)
                  {
                    it.Value() = one;
                    break;
                  }
              }
          }
        ++bit;
        ++it;
      }
    
    }

  
}

}//end of itk namespace



#endif
