/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit (ITK)
  Module:    itkVTKImageIOTest.cxx
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
#include <iostream>
#include "itkImage.h"
#include "itkVector.h"
#include "itkRandomImageSource.h"
#include "itkShrinkImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkVTKImageIO.h"
#include "itkCommand.h"
#include "itkOutputWindow.h"


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
// on the shrink filter in the pipeline that follows later
class ShowProgressObject
{
public:
  ShowProgressObject(itk::ProcessObject* o)
    {m_Process = o;}
  void ShowProgress()
    {std::cout << "Progress " << m_Process->GetProgress() << std::endl;}
  itk::ProcessObject::Pointer m_Process;
};
  
class StartEndEvent
{
public:
  void Start() 
    {std::cout << "start event" << std::endl;}
  void End()
    {std::cout << "end event " << std::endl;}
};


class AllEvents
{
public:
  void WatchEvents(itk::Object *caller, const itk::EventObject & event )
    {
      const char* eventName = 0;
      if( typeid( event ) == typeid( itk::DeleteEvent ) )
        {
        eventName = "DeleteEvent";
        }
      else if( typeid( event ) == typeid( itk::StartEvent ) )
        {
        eventName = "StartEvent";
        }
      else if( typeid( event ) == typeid( itk::EndEvent ) )
        {
        eventName = "EndEvent";
        }
      else if( typeid( event ) == typeid( itk::ProgressEvent ) )
        {
        itk::ProcessObject* obj = dynamic_cast<itk::ProcessObject*>(caller);
        std::cout << "AnyEvent Progress " << obj->GetProgress() << std::endl;
        eventName = "ProgressEvent";
          }
      else if( typeid( event ) == typeid( itk::PickEvent ) )
        {
        eventName = "PickEvent";
        }
      else if( typeid( event ) == typeid( itk::StartPickEvent ) )
        {
        eventName = "StartPickEvent";
        }
      else if( typeid( event ) == typeid( itk::AbortCheckEvent ) )
        {
        eventName = "AbortCheckEvent";
        }
      else if( typeid( event ) == typeid( itk::ExitEvent ) )
        {
        eventName = "ExitEvent";
        }
      else 
        {
        eventName = "UserEvent";
        }
      std::cout << "Event name: " << eventName << " Id: " << event.GetEventName() << std::endl;
    }
};


int main()
{
  // Comment the following if you want to use the itk text output window
  itk::OutputWindow::SetInstance(TextOutput::New().GetPointer());
  // Uncomment the following if you want to see each message independently
  // itk::OutputWindow::GetInstance()->PromptUserOn();

  // Test the creation of an image with native type
  //
  itk::Image<float,2>::Pointer if2 = itk::Image<float,2>::New();

  std::cout << std::endl
            << "Image dimension is " << itk::Image<float,5>::ImageDimension
            << std::endl;
  std::cout << "Image dimension is " << itk::Image<short,1>::ImageDimension
            << std::endl;

  // Begin by creating a simple pipeline. Use a scalar ss a pixel.
  //
  // Create a typedef to make the code more digestable
  typedef itk::Image<float,2> FloatImage2DType;

  // Create a source object (in this case a reader)
  itk::ImageFileReader<FloatImage2DType>::Pointer reader;
  reader = itk::ImageFileReader<FloatImage2DType>::New();
  reader->SetFileName("junkInput.vtk");

  // Create another source object (in this case a random image generator).
  // The source object is templated on the output type.
  //
  itk::RandomImageSource<FloatImage2DType>::Pointer random;
  random = itk::RandomImageSource<FloatImage2DType>::New();
  random->SetMin(0.0);
  random->SetMax(1.0);

  // Create a filter...shrink the image by an integral amount. We also 
  // add some callbacks to the start, progress, and end filter execution
  // methods. The filter is templated on the input and output data types.
  //
  itk::ShrinkImageFilter<FloatImage2DType,FloatImage2DType>::Pointer shrink;
  shrink = itk::ShrinkImageFilter<FloatImage2DType,FloatImage2DType>::New();
  shrink->SetInput(random->GetOutput());
  shrink->SetShrinkFactors(2);
  
  // Create a command to call ShowProgress when progress event is triggered
  ShowProgressObject progressWatch(shrink);
  itk::SimpleMemberCommand<ShowProgressObject>::Pointer command;
  command = itk::SimpleMemberCommand<ShowProgressObject>::New();
  command->SetCallbackFunction(&progressWatch,
                               &ShowProgressObject::ShowProgress);
  shrink->AddObserver(itk::ProgressEvent(), command);
  
  // Create a command to call StartEndEvent when start event is triggered
  StartEndEvent startEndWatch;
  itk::SimpleMemberCommand<StartEndEvent>::Pointer start;
  start = itk::SimpleMemberCommand<StartEndEvent>::New();
  start->SetCallbackFunction(&startEndWatch, &StartEndEvent::Start);
  unsigned long tag = shrink->AddObserver(itk::StartEvent(), start);
  
  // Create a command to call StartEndEvent when end event is triggered
  itk::SimpleMemberCommand<StartEndEvent>::Pointer end;
  end = itk::SimpleMemberCommand<StartEndEvent>::New();
  end->SetCallbackFunction(&startEndWatch, &StartEndEvent::End);
  shrink->AddObserver(itk::EndEvent(), end);
  
  // Create a command that to call AnyEvent when event is fired
  AllEvents allWatch;
  itk::MemberCommand<AllEvents>::Pointer allEvents;
  allEvents = itk::MemberCommand<AllEvents>::New();
  allEvents->SetCallbackFunction(&allWatch,
                                 &AllEvents::WatchEvents);
  shrink->AddObserver(itk::AnyEvent(), allEvents);
  
  // Create a mapper (in this case a writer). A mapper
  // is templated on the input type.
  //
  itk::ImageFileWriter<FloatImage2DType>::Pointer writer;
  writer = itk::ImageFileWriter<FloatImage2DType>::New();
  writer->SetInput(shrink->GetOutput());
  writer->SetFileName("BasicArchitectureImage.vtk");
  writer->Write();

  // test RemoveObserver code
  shrink->RemoveObserver( tag );

  return EXIT_SUCCESS;
}



