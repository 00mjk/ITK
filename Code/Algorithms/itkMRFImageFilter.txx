/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkMRFImageFilter.txx
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
#ifndef _itkMRFImageFilter_txx
#define _itkMRFImageFilter_txx

namespace itk
{

template<class TInputImage, class TClassifiedImage>
MRFImageFilter<TInputImage,TClassifiedImage>
::MRFImageFilter(void):
      m_InputImage(0),
      m_TrainingImage(0),
      m_LabelledImage(0),
      m_NumberOfClasses(0),
      m_MaximumNumberOfIterations(50),
      m_LabelStatus(0),
      m_ErrorTolerance(0),
      m_ClassProbability(0),
      m_Beta3x3x3(0),
      m_ClassifierPtr(0),
      m_ErrorCounter(0),
      m_Offset(0),
      m_kWidth(3), // Default values of the kernel size of the beta matrix
      m_kHeight(3), 
      m_kDepth(3)
{
  m_KernelSize = m_kWidth * m_kHeight * m_kDepth;
  SetBeta( 0 );
}

template<class TInputImage, class TClassifiedImage>
MRFImageFilter<TInputImage, TClassifiedImage>
::~MRFImageFilter(void)
{
  delete [] m_WidthOffset;
  delete [] m_HeightOffset;
  delete [] m_DepthOffset;
  delete [] m_Beta3x3x3;
  if( m_LabelStatus ) delete [] m_LabelStatus;
}

/**
 * GenerateInputRequestedRegion method.
 */
template <class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::GenerateInputRequestedRegion()
{

  // this filter requires the all of the input image to be in
  // the buffer
  InputImageType inputPtr = this->GetInput();
  inputPtr->SetRequestedRegionToLargestPossibleRegion();
}


/**
 * EnlargeOutputRequestedRegion method.
 */
template <class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::EnlargeOutputRequestedRegion(
DataObject *output )
{

  // this filter requires the all of the output image to be in
  // the buffer
  TClassifiedImage *imgData;
  imgData = dynamic_cast<TClassifiedImage*>( output );
  imgData->SetRequestedRegionToLargestPossibleRegion();

}

/**
 * GenerateOutputInformation method.
 */
template <class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::GenerateOutputInformation()
{
  typename TInputImage::Pointer input = this->GetInput();
  typename TClassifiedImage::Pointer output = this->GetOutput();
  output->SetLargestPossibleRegion( input->GetLargestPossibleRegion() );

}

template<class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::GenerateData()
{
  //First run the Gaussian classifier calculator and
  //generate the Gaussian model for the different classes
  //and then generate the initial labelled dataset.

  m_InputImage = this->GetInput();
    
  //Give the input image and training image set to the  
  // classifier
  m_ClassifierPtr->SetInputImage( m_InputImage );
  m_ClassifierPtr->SetTrainingImage( m_TrainingImage );

  //Run the gaussian classifier algorithm
  m_ClassifierPtr->ClassifyImage();

  SetLabelledImage( m_ClassifierPtr->GetClassifiedImage() );

  ApplyMRFImageFilter();
  //Set the output labelled and allocate the memory
  LabelledImageType outputPtr = this->GetOutput();

  //Allocate the output buffer memory 
  outputPtr->SetBufferedRegion( outputPtr->GetRequestedRegion() );
  outputPtr->Allocate();

  //--------------------------------------------------------------------
  //Copy labelling result to the output buffer
  //--------------------------------------------------------------------
  // Set the iterators to the processed image
  //--------------------------------------------------------------------
  LabelledImageIterator  
    labelledImageIt( m_LabelledImage, m_LabelledImage->GetBufferedRegion() );

  //--------------------------------------------------------------------
  // Set the iterators to the output image buffer
  //--------------------------------------------------------------------
  LabelledImageIterator  
    outImageIt( outputPtr, outputPtr->GetBufferedRegion() );

  //--------------------------------------------------------------------

  while ( !outImageIt.IsAtEnd() )
    {
    LabelledImagePixelType labelvalue = 
      ( LabelledImagePixelType ) labelledImageIt.Get();

    outImageIt.Set( labelvalue );
    ++labelledImageIt;
    ++outImageIt;
    }// end while
        
}// end GenerateData

template<class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::SetClassifier( typename ClassifierType::Pointer ptrToClassifier )
{
  if( ( ptrToClassifier == 0 ) || (m_NumberOfClasses <= 0) )
    throw ExceptionObject(__FILE__, __LINE__);

  m_ClassifierPtr = ptrToClassifier;
  m_ClassifierPtr->SetNumberOfClasses( m_NumberOfClasses );
}//end SetPtrToClassifier

template<class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::SetTrainingImage( TrainingImageType image )
{
  m_TrainingImage = image;
}//end SetInputImage



template<class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::SetLabelledImage(LabelledImageType image)
{
  m_LabelledImage = image;
  this->Allocate();
}// Set the LabelledImage

template<class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
}// end PrintSelf


template<class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::Allocate()
{
  if( m_NumberOfClasses <= 0 )
    {
    throw ExceptionObject(__FILE__, __LINE__);
    }

  InputImageSizeType inputImageSize = m_InputImage->GetBufferedRegion().GetSize();

  //Ensure that the data provided is three dimensional or higher data set
  if(TInputImage::ImageDimension <= 2 )
    {
    throw ExceptionObject(__FILE__, __LINE__);
    }
  
  //---------------------------------------------------------------------
  //Get the image width/height and depth
  //---------------------------------------------------------------------       
  m_imgWidth  = static_cast<int>(inputImageSize[0]);
  m_imgHeight = static_cast<int>(inputImageSize[1]);
  m_imgDepth  = static_cast<int>(inputImageSize[2]);
 
  m_LabelStatus = new unsigned int[m_imgWidth*m_imgHeight*m_imgDepth]; 
  for( int index = 0; 
       index < ( m_imgWidth * m_imgHeight * m_imgDepth ); 
       index++ ) 
    {
    m_LabelStatus[index]=1;
    }

}// Allocate


template<class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::SetBeta( double* )
{

  // Set the beta matrix of a 3x3x3 kernel
  // The index starts from 0 going along the three dimensions
  // in the order of [coloumn], [row], [depth].

  //Allocate memory for the weights of the 3D MRF algorithm
  // and corresponding memory offsets.
  m_WidthOffset     = (int *)   new int   [m_KernelSize];
  m_HeightOffset    = (int *)   new int   [m_KernelSize];
  m_DepthOffset     = (int *)   new int   [m_KernelSize];
  m_Beta3x3x3       = (double *)new double[m_KernelSize];


  for( int i = 0; i < 9; i++ ) 
    m_Beta3x3x3[i] = 1.3;

  for( int i = 9; i < 18; i++ )
    m_Beta3x3x3[i] = 1.7;

  for( int i = 18; i < 27; i++ )
    m_Beta3x3x3[i] = 1.3;

  // Change the center weights
  m_Beta3x3x3[4]  = 1.5;
  m_Beta3x3x3[13] = 0.0;
  m_Beta3x3x3[22] = 1.5;

  // k prefix stands for the kernel
        
  int kHalfWidth  = m_kWidth/2;
  int kHalfHeight = m_kHeight/2;
  int kHalfDepth  = m_kDepth/2;

  int l=0; // index for the offset 

  // Now calculate the corresponding offsets
  for( int k = 0; k < m_kDepth; k++ )
    {
    for( int j = 0; j < m_kHeight; j++ )
      {
      for( int i = 0; i < m_kWidth; i++, l++ )
        {
        m_WidthOffset[l]  = ( i - kHalfWidth ); 
        m_HeightOffset[l] = ( j - kHalfHeight );
        m_DepthOffset[l]  = ( k - kHalfDepth );
        }// end for (width loop)
      }// end for (height loop)
    }// end for (depth loop)

}// SetBeta


template<class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::SetBeta( double *betaMatrix, unsigned int kernelSize )
{
  m_KernelSize = kernelSize;
  //Allocate memory for the weights of the 3D MRF algorithm
  // and corresponding memory offsets.
  m_WidthOffset     = (int *)   new int   [m_KernelSize];
  m_HeightOffset    = (int *)   new int   [m_KernelSize];
  m_DepthOffset     = (int *)   new int   [m_KernelSize];
  m_Beta3x3x3       = (double *)new double[m_KernelSize];

  for( unsigned int i = 0; i < m_KernelSize; i++ ) 
    m_Beta3x3x3[i] = *betaMatrix++;
}// end SetBeta

template<class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::SetBeta( vnl_vector<double> betaMatrix)
{
  m_KernelSize = betaMatrix.size();
  //Allocate memory for the weights of the 3D MRF algorithm
  // and corresponding memory offsets.
  m_WidthOffset     = (int *)   new int   [m_KernelSize];
  m_HeightOffset    = (int *)   new int   [m_KernelSize];
  m_DepthOffset     = (int *)   new int   [m_KernelSize];
  m_Beta3x3x3       = (double *)new double[m_KernelSize];

  for( unsigned int i = 0; i < betaMatrix.size(); i++ ) 
    m_Beta3x3x3[i] = betaMatrix[i];
}// end SetBeta


template<class TInputImage, class TClassifiedImage>
void 
MRFImageFilter<TInputImage, TClassifiedImage>
::ApplyMRFImageFilter()
{

  int maxNumPixelError =  
    (int)(m_ErrorTolerance * m_imgWidth * m_imgHeight * m_imgDepth);

  unsigned int numIter = 0;
  do
    {
    itkDebugMacro(<< "Iteration No." << numIter);

    m_ErrorCounter = 0;
    MinimizeFunctional();
    numIter += 1;

    for(int index=0; 
        index<( m_imgWidth * m_imgHeight * m_imgDepth ); index++ )
      {
      if(m_LabelStatus[index] == 1) m_ErrorCounter +=1;
      }
    } 
  while(( numIter < m_MaximumNumberOfIterations ) && 
        ( m_ErrorCounter >maxNumPixelError ) )
    ; 

}// ApplyMRFImageFilter

template<class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::MinimizeFunctional()
{
  //This implementation uses the ICM algorithm
  ApplyICMLabeller();
}

template<class TInputImage, class TClassifiedImage>
void
MRFImageFilter<TInputImage, TClassifiedImage>
::ApplyICMLabeller()
{
  //--------------------------------------------------------------------
  // Set the iterators and the pixel type definition for the input image
  //-------------------------------------------------------------------
  InputImageIterator  inputImageIt(m_InputImage, 
                                   m_InputImage->GetBufferedRegion() );

  //--------------------------------------------------------------------
  // Set the iterators and the pixel type definition for the classified image
  //--------------------------------------------------------------------
  LabelledImageIterator  
    labelledImageIt(m_LabelledImage, m_LabelledImage->GetBufferedRegion());

  //---------------------------------------------------------------------
  // Loop through the data set and classify the data
  //---------------------------------------------------------------------

  int offset;
  double *neighborInfluence = (double *) new double[m_NumberOfClasses + 1];

  //Varible to store the input pixel vector value
  InputImageVectorType inputPixelVec;

  //Variable to store the labelled pixel vector
  LabelledImagePixelType labelledPixel;

  //Variable to store the output pixel vector label after
  //the MRF classification
  LabelledImagePixelType outLabelledPix;

  //Set a variable to store the offset
  LabelledImageOffsetType offset3D;

  //Set a variable to store the index
  LabelledImageIndexType index3D;

  int imageFrame = m_imgWidth * m_imgHeight;

  double * dist = new double[m_NumberOfClasses];
 
  for( int d = 0; d < m_imgDepth; d++ )
    {
    for( int j = 0; j < m_imgHeight; j++ )
      {
      for( int i = 0; i < m_imgWidth; i++, ++inputImageIt, ++labelledImageIt )
        {
        int labelStatusPtrOffset = i + j * m_imgHeight + d * imageFrame;

        //Check if the label == 1 indicates the need for pixel reclassification

        //Uncheck the comment after consulting with Vikram, 
        //Current status returns the best possible result with the
        //flag check suspended

        inputPixelVec = inputImageIt.Get();
        m_ClassifierPtr->GetPixelDistance( inputPixelVec, dist );
                      
        for( unsigned int index = 0; index <= m_NumberOfClasses ;index++ ) 
          neighborInfluence[index]=0;

        for(unsigned int k=0;k<m_KernelSize;k++)
          {
          int widthOffset  = i + m_WidthOffset[k];
          int heightOffset = j + m_HeightOffset[k];
          int depthOffset  = d + m_DepthOffset[k];

          if( ( widthOffset >= 0 ) && ( widthOffset < m_imgWidth ) &&
              ( heightOffset >= 0 ) && ( heightOffset < m_imgHeight ) &&
              ( depthOffset >= 0) && ( depthOffset <m_imgDepth ) )
            {
            //More elegant solution would be to use neighborhood
            //operators but it is currently unstable for VC++

            offset3D[0] = m_WidthOffset[k] ;
            offset3D[1] = m_HeightOffset[k] ;
            offset3D[2] = m_DepthOffset[k]  ;
            index3D = labelledImageIt.GetIndex();
	    index3D += offset3D;

            labelledPixel = m_LabelledImage->GetPixel( index3D );
                
            //Assuems that the MRF label is an image with 1 value
            //per pixel and is treated as a vector with 1 entry pervector
            int index = (int) labelledPixel;
    
            //Do the prior probability calculations for each class
            //in the 3x3x3 neighborhood
            neighborInfluence[index] += m_Beta3x3x3[k];       

            }// end if

          }// end for 3x3x3 neighborhood processing

        for( unsigned int index = 1; index <= m_NumberOfClasses; index++ )
          dist[index - 1] = neighborInfluence[index] - dist[index - 1] ;

        double maxDist = -1e+20;
        int pixLabel = -1;

        for( unsigned int index = 0; index < m_NumberOfClasses; index++ )
          {
          if ( dist[index] > maxDist )
            {
            maxDist = dist[index];
            pixLabel = index;
            }// if
          }// for
                      
        //Read the current pixel label
        labelledPixel = 
          ( LabelledImagePixelType ) labelledImageIt.Get();

        //Check if the label has changed then set the change flag in all the 
        //neighborhood of the current pixel
        if( pixLabel != ( int ) labelledPixel )
          {
          //While the distance array starts with 0, the actual index starts from 1
          outLabelledPix = pixLabel;
          labelledImageIt.Set( outLabelledPix );

          for( unsigned int k = 0; k < m_KernelSize; k++ )
            {
            int widthOffset  = i + m_WidthOffset[k];
            int heightOffset = j + m_HeightOffset[k];
            int depthOffset  = d + m_DepthOffset[k];

            if( ( widthOffset >= 0) && ( widthOffset < m_imgWidth ) &&
                ( heightOffset >= 0) && ( heightOffset < m_imgHeight ) &&
                ( depthOffset >= 0) && ( depthOffset < m_imgDepth ) )
              {

              offset = m_WidthOffset[k] +
                m_HeightOffset[k] * m_imgWidth +
                m_DepthOffset[k] * imageFrame;
                  
              m_LabelStatus[labelStatusPtrOffset +offset] = 1;
              }// end if

            }// end for 3x3x3 neighborhood processing
          } //if
        else 
          {
          m_LabelStatus[labelStatusPtrOffset]=0;
          }// else         

        }//end imageWidth
      }// end imageHeight
    }// end imageDepth

  delete [] dist;
  delete[] neighborInfluence;
}//ApplyICMlabeller

} // namespace itk








#endif
