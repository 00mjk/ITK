/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit (ITK)
  Module:    itkCurvatureFlowTest.cxx
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

#include "itkCurvatureFlowImageFilter.h"
#include "itkRandomImageSource.h"
#include "itkImage.h"
#include "itkVTKImageWriter.h"
#include "itkOutputWindow.h"
#include "itkCommand.h"
#include "itkCastImageFilter.h"
#include "itkStreamingImageFilter.h"
#include "itkImageRegionIterator.h"


// this class is used to send output to stdout and not the itk window
class TextOutput : public itk::OutputWindow
{
public:
  typedef TextOutput              Self;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;
  itkNewMacro(TextOutput);
  virtual void DisplayText(const char* s)
    {
      std::cout << s << std::endl;
    }
};


// The following three classes are used to support callbacks
// on the filter in the pipeline that follows later
class ShowProgressObject
{
public:
  ShowProgressObject(itk::ProcessObject* o)
    {m_Process = o;}
  void ShowProgress()
    {std::cout << "Progress " << m_Process->GetProgress() << std::endl;}
  itk::ProcessObject::Pointer m_Process;
};


int main()
{

   itk::OutputWindow::SetInstance(TextOutput::New().GetPointer());


  typedef float PixelType;
  enum { ImageDimension = 2 };
  typedef itk::Image<PixelType, ImageDimension> ImageType;

  std::cout << "Create input image using RandomImageSource" << std::endl;
  typedef itk::RandomImageSource<ImageType> SourceType;

  SourceType::Pointer source = SourceType::New();

  unsigned long size[ImageDimension] = {64,64};
  source->SetSize( size );
  source->SetMin(0.0);
  source->SetMax(1.0);
  source->Update();

  std::cout << "Run CurvatureFlowImageFiler with progress cout's" << std::endl;
  typedef itk::CurvatureFlowImageFilter<ImageType,ImageType> DenoiserType;
  DenoiserType::Pointer denoiser = DenoiserType::New();

  denoiser->SetInput( source->GetOutput() );
  denoiser->SetTimeStep( 0.15 );
  denoiser->SetNumberOfIterations( 8 );

  ShowProgressObject progressWatch(denoiser);
  itk::SimpleMemberCommand<ShowProgressObject>::Pointer command;
  command = itk::SimpleMemberCommand<ShowProgressObject>::New();
  command->SetCallbackFunction(&progressWatch,
                               &ShowProgressObject::ShowProgress);
  denoiser->AddObserver(itk::Command::ProgressEvent, command);

  denoiser->Update();


  std::cout << "Run CurvatureFlowImageFilter using streamer" << std::endl;
  typedef itk::CastImageFilter<ImageType,ImageType> CasterType;
  CasterType::Pointer caster = CasterType::New();
  caster->SetInput( denoiser->GetInput() );

  DenoiserType::Pointer denoiser2 = DenoiserType::New();
  denoiser2->SetInput( caster->GetOutput() );
  denoiser2->SetTimeStep( denoiser->GetTimeStep() );
  denoiser2->SetNumberOfIterations( denoiser->GetNumberOfIterations() );

  typedef itk::StreamingImageFilter<ImageType,ImageType> StreamerType;
  StreamerType::Pointer streamer = StreamerType::New();
  streamer->SetInput( denoiser2->GetOutput() );
  streamer->SetNumberOfStreamDivisions( 3 );
  streamer->Update();

  std::cout << "Compare stand-alone and streamer outputs" << std::endl;
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  IteratorType it1( denoiser->GetOutput(),
    denoiser->GetOutput()->GetBufferedRegion() );
  IteratorType it2( streamer->GetOutput(),
    streamer->GetOutput()->GetBufferedRegion() );

  bool testPass = true;
  while( !it1.IsAtEnd() )
    {
    if( it1.Get() != it2.Get() )
      { testPass = false; }
    ++it1;
    ++it2;
    }

  if( !testPass )
    {
    std::cout << "Test failed." << std::endl;
    return EXIT_FAILURE;
    }
 
  typedef itk::VTKImageWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();

  writer->SetInput( streamer->GetOutput() );
  writer->SetFileName("CurvatureFlowImageFilterImage.vtk");
//  writer->SetFileTypeToBinary();
  writer->Write();


  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;

}
