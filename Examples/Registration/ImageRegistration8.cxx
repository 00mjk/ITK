/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageRegistration8.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// Software Guide : BeginLatex
//
// This example illustrates the use of the \doxygen{VersorRigid3DTransform}
// class for performing registration of two $3D$ images. The example code is
// for the most part identical to the code presented in
// Section~\ref{sec:RigidRegistrationIn2D}.  The major difference is that this
// example is done in $3D$. The class \doxygen{CenteredTransformInitializer} is
// used to initialize the center and translation of the transform.  The case of
// rigid registration of 3D images is probably one of the most commonly found
// casses of image registration.
//
//
// \index{itk::VersorRigid3DTransform}
// \index{itk::CenteredTransformInitializer!In 3D}
//
//
// Software Guide : EndLatex 

#include "itkImageRegistrationMethod.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"


//  Software Guide : BeginLatex
//  
//  The following are the most relevant headers to this example.
//
//  \index{itk::VersorRigid3DTransform!header}
//  \index{itk::CenteredTransformInitializer!header}
// 
//  Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "itkVersorRigid3DTransform.h"
#include "itkCenteredTransformInitializer.h"
// Software Guide : EndCodeSnippet

//  Software Guide : BeginLatex
//  
//  The parameter space of the \code{VersorRigid3DTransform} is not a vector
//  space, due to the fact that addition is not a closed operation in the space
//  of versor components. This precludes the use of standard gradient descent
//  algorithms for optimizing the parameter space of this transform. A special
//  optimizer should be used in this registration configuration. The optimizer
//  designed for this transform is the
//  \doxygen{VersorRigid3DTransformOptimizer}. This optimizer uses Versor
//  composition for updating the first three components of the parameters
//  array, and Vector addition for updating the last three components of the
//  parameters array.
//
//  \index{itk::VersorRigid3DTransformOptimizer!header}
// 
//  Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "itkVersorRigid3DTransformOptimizer.h"
// Software Guide : EndCodeSnippet

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkSquaredDifferenceImageFilter.h"


//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//
#include "itkCommand.h"
class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  typedef itk::VersorRigid3DTransformOptimizer     OptimizerType;
  typedef   const OptimizerType   *    OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( object );
      if( typeid( event ) != typeid( itk::IterationEvent ) )
        {
        return;
        }
      std::cout << optimizer->GetCurrentIteration() << "   ";
      std::cout << optimizer->GetValue() << "   ";
      std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};


int main( int argc, char *argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFile  movingImageFile ";
    std::cerr << " outputImagefile  [differenceOutputfile] ";
    std::cerr << " [differenceBeforeRegistration] "<< std::endl;
    return 1;
    }
  
  const    unsigned int    Dimension = 3;
  typedef  float           PixelType;

  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;


  //  Software Guide : BeginLatex
  //  
  //  The Transform class is instantiated using the code below. The only
  //  template parameter to this class is the representation type of the
  //  space coordinates.
  //
  //  \index{itk::VersorRigid3DTransform!Instantiation}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef itk::VersorRigid3DTransform< double > TransformType;
  // Software Guide : EndCodeSnippet



  typedef itk::VersorRigid3DTransformOptimizer           OptimizerType;

  typedef itk::MeanSquaresImageToImageMetric< 
                                    FixedImageType, 
                                    MovingImageType >    MetricType;

  typedef itk:: LinearInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    InterpolatorType;

  typedef itk::ImageRegistrationMethod< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;

  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();
  

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );


  //  Software Guide : BeginLatex
  //
  //  The transform object is constructed below and passed to the registration
  //  method.
  //
  //  \index{itk::VersorRigid3DTransform!New()}
  //  \index{itk::VersorRigid3DTransform!Pointer}
  //  \index{itk::RegistrationMethod!SetTransform()}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  TransformType::Pointer  transform = TransformType::New();
  registration->SetTransform( transform );
  // Software Guide : EndCodeSnippet

  typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;

  FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  fixedImageReader->SetFileName(  argv[1] );
  movingImageReader->SetFileName( argv[2] );

  registration->SetFixedImage(    fixedImageReader->GetOutput()    );
  registration->SetMovingImage(   movingImageReader->GetOutput()   );
  fixedImageReader->Update();

  registration->SetFixedImageRegion( 
     fixedImageReader->GetOutput()->GetBufferedRegion() );


  //  Software Guide : BeginLatex
  //  
  //  The input images are taken from readers. It is not necessary here to
  //  explicitly call \code{Update()} on the readers since the
  //  \doxygen{CenteredTransformInitializer} will do it as part of its
  //  computations. The following code instantiates the type of the
  //  initializer. This class is templated over the fixed and moving image type
  //  as well as the transform type. An initializer is then constructed by
  //  calling the \code{New()} method and assigning the result to a smart
  //  pointer.
  //
  // \index{itk::CenteredTransformInitializer!Instantiation}
  // \index{itk::CenteredTransformInitializer!New()}
  // \index{itk::CenteredTransformInitializer!SmartPointer}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef itk::CenteredTransformInitializer< TransformType, 
                                             FixedImageType, 
                                             MovingImageType 
                                                 >  TransformInitializerType;

  TransformInitializerType::Pointer initializer = 
                                          TransformInitializerType::New();
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The initializer is now connected to the transform and to the fixed and
  //  moving images.
  //
  //  Software Guide : EndLatex 


  // Software Guide : BeginCodeSnippet
  initializer->SetTransform(   transform );
  initializer->SetFixedImage(  fixedImageReader->GetOutput() );
  initializer->SetMovingImage( movingImageReader->GetOutput() );
  // Software Guide : EndCodeSnippet

  //  Software Guide : BeginLatex
  //
  //  The use of the geometrical centers is selected by calling
  //  \code{GeometryOn()} while the use of center of mass is selected by
  //  calling \code{MomentsOn()}.  Below we select the center of mass mode.
  //
  //  \index{CenteredTransformInitializer!MomentsOn()}
  //  \index{CenteredTransformInitializer!GeometryOn()}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  initializer->MomentsOn();
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  Finally, the computation of the center and translation is triggered by
  //  the \code{InitializeTransform()} method. The resulting values will be
  //  passed directly to the transform.
  //
  //  Software Guide : EndLatex 


  // Software Guide : BeginCodeSnippet
  initializer->InitializeTransform();
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //  
  //  The rotation part of the transform is initialized using a
  //  \doxygen{Versor} which is simply a unit quaternion.  The
  //  \code{VersorType} can be obtained from the transform traits. The versor
  //  itself defines the type of the vector used to indicate the rotation axis.
  //  This trait can be extracted as \code{VectorType}. The following lines
  //  create a versor object and initialize its parameters by passing a
  //  rotation axis and an angle.
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef TransformType::VersorType  VersorType;
  typedef VersorType::VectorType     VectorType;

  VersorType     rotation;
  VectorType     axis;
  
  axis[0] = 0.0;
  axis[1] = 0.0;
  axis[2] = 1.0;

  const double angle = 0;

  rotation.Set(  axis, angle  );

  transform->SetRotation( rotation );
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //  
  //  We now pass the parameters of the current transform as the initial
  //  parameters to be used when the registration process starts. 
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  registration->SetInitialTransformParameters( transform->GetParameters() );
  // Software Guide : EndCodeSnippet

  std::cout << "Intial Parameters = " << std::endl;
  std::cout << transform->GetParameters() << std::endl;

  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );
  const double translationScale = 1.0 / 1000.0;

  optimizerScales[0] = 1.0;
  optimizerScales[1] = 1.0;
  optimizerScales[2] = 1.0;
  optimizerScales[3] = translationScale;
  optimizerScales[4] = translationScale;
  optimizerScales[5] = translationScale;

  optimizer->SetScales( optimizerScales );

  optimizer->SetMaximumStepLength( 1.000  ); 
  optimizer->SetMinimumStepLength( 0.001 );

  optimizer->SetNumberOfIterations( 200 );


  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  std::cout << std::endl << "Starting Registration" << std::endl;

  try 
    { 
    registration->StartRegistration(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return -1;
    } 
  
  OptimizerType::ParametersType finalParameters = 
                    registration->GetLastTransformParameters();


  const double versorX              = finalParameters[0];
  const double versorY              = finalParameters[1];
  const double versorZ              = finalParameters[2];
  const double finalTranslationX    = finalParameters[3];
  const double finalTranslationY    = finalParameters[4];
  const double finalTranslationZ    = finalParameters[5];

  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

  const double bestValue = optimizer->GetValue();


  // Print out results
  //
  std::cout << "Result = " << std::endl;
  std::cout << " versor X      = " << versorX  << std::endl;
  std::cout << " versor Y      = " << versorY  << std::endl;
  std::cout << " versor Z      = " << versorZ  << std::endl;
  std::cout << " Translation X = " << finalTranslationX  << std::endl;
  std::cout << " Translation Y = " << finalTranslationY  << std::endl;
  std::cout << " Translation Z = " << finalTranslationZ  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;


  //  Software Guide : BeginLatex
  //  
  //  Let's execute this example over some of the images available in
  //  the ftp site, for example:
  //  
  //  \begin{itemize}
  //  \item \code{brainweb165a10f17.mha} 
  //  \item \code{brainweb165a10f17Rot10Tx15.mha}
  //  \end{itemize}
  //
  //  The second image is the result of intentionally rotating the first image
  //  by $10$ degrees around the origin and shifting it $15mm$ in $X$.  The
  //  registration takes $19$ iterations and produces:
  //
  //  \begin{center}
  //  \begin{verbatim}
  //  [-2.84486e-05, 5.73525e-05, -0.0870955, -0.112467, -17.5025, -0.00222232]
  //  \end{verbatim}
  //  \end{center}
  //
  //  That are interpreted as
  //
  //  \begin{itemize}
  //  \item Versor        = $(-2.84e-05, 5.73e-05, -0.08709, 0.9962 )$
  //  \item Translation   = $(  -0.1124, -17.5025, -0.0022    )$ millimeters
  //  \end{itemize}
  //  
  //  This Versor is equivalent to a rotaion of $9.98$ degrees around the $Z$
  //  axis.
  // 
  //  Note that the reported translation is not the translation of $(15.0,0.0,0.0)$
  //  that we may be naively expecting. The reason is that the
  //  \code{VersorRigid3DTransform} is applying the rotation around the center
  //  found by the \code{CenteredTransformInitializer} and then adding the
  //  translation vector shown above.
  //
  //  It is more
  //  illustrative in this case to take a look at the actual rotation matrix
  //  and offset resulting form the $5$ parameters.
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  transform->SetParameters( finalParameters );

  TransformType::MatrixType matrix = transform->GetRotationMatrix();
  TransformType::OffsetType offset = transform->GetOffset();

  std::cout << "Matrix = " << std::endl << matrix << std::endl;
  std::cout << "Offset = " << std::endl << offset << std::endl;
  // Software Guide : EndCodeSnippet

#define TUMBUKTU

  //  Software Guide : BeginLatex
  //
  //  You may be wondering why if the actual movement is represented by three
  //  parameters this example uses five instead. The answer is that by using
  //  five parameters it is easier to initialize the transform with an
  //  appropriate rotation matrix and offset. Using the minimum three
  //  parameters is not obvious how to find what the rotation and
  //  translations should be.
  //
  //  Software Guide : EndLatex 


  //  Software Guide : BeginLatex
  //  
  // \begin{figure}
  // \center
  // \includegraphics[width=0.44\textwidth]{BrainProtonDensitySliceBorder20.eps}
  // \includegraphics[width=0.44\textwidth]{BrainProtonDensitySliceR10X13Y17.eps}
  // \itkcaption[CenteredTransformInitializer input images]{Fixed and moving image
  // provided as input to the registration method using
  // CenteredTransformInitializer.}
  // \label{fig:FixedMovingImageRegistration8}
  // \end{figure}
  //
  //
  // \begin{figure}
  // \center
  // \includegraphics[width=0.32\textwidth]{ImageRegistration8Output.eps}
  // \includegraphics[width=0.32\textwidth]{ImageRegistration8DifferenceBefore.eps}
  // \includegraphics[width=0.32\textwidth]{ImageRegistration8DifferenceAfter.eps} 
  // \itkcaption[CenteredTransformInitializer output images]{Resampled moving
  // image (left). Differences between fixed and moving images, before (center)
  // and after (right) registration with the
  // CenteredTransformInitializer.}
  // \label{fig:ImageRegistration8Outputs}
  // \end{figure}
  //
  // Figure \ref{fig:ImageRegistration8Outputs} shows the output of the
  // registration. The right most image of this figure shows the squared
  // magnitude of pixel differences between the fixed image and the resampled
  // moving image. 
  //
  // \begin{figure}
  // \center
  // \includegraphics[height=0.32\textwidth]{ImageRegistration8TraceMetric.eps}
  // \includegraphics[height=0.32\textwidth]{ImageRegistration8TraceAngle.eps}
  // \includegraphics[height=0.32\textwidth]{ImageRegistration8TraceTranslations.eps} 
  // \itkcaption[CenteredTransformInitializer output plots]{Plots of the metric,
  // rotation angle, center of rotation and translations during the
  // registration using CenteredTransformInitializer.}
  // \label{fig:ImageRegistration8Plots}
  // \end{figure}
  //
  //  Figure \ref{fig:ImageRegistration8Plots} shows the plots of the main
  //  output parameters of the registration process. The metric values at every
  //  iteration are shown on the top. The angle values are shown in the plot at
  //  left while the translation components of the registration are presented
  //  in the plot at right. Note that this is the complementary translation as
  //  used in the transform, not the atctual total translation that gets used
  //  in the offset of the transform. We could modify the Observer in order to
  //  print the total offset instead of printing the array of parameters. Let's
  //  call that an exercise for the reader!
  //
  //  Software Guide : EndLatex 


  typedef itk::ResampleImageFilter< 
                            MovingImageType, 
                            FixedImageType >    ResampleFilterType;

  TransformType::Pointer finalTransform = TransformType::New();

  finalTransform->SetParameters( finalParameters );

  ResampleFilterType::Pointer resample = ResampleFilterType::New();

  resample->SetTransform( finalTransform );
  resample->SetInput( movingImageReader->GetOutput() );

  FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();

  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetDefaultPixelValue( 100 );
  
  typedef  unsigned char  OutputPixelType;

  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  
  typedef itk::CastImageFilter< 
                        FixedImageType,
                        OutputImageType > CastFilterType;
                    
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;


  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster =  CastFilterType::New();


  writer->SetFileName( argv[3] );


  caster->SetInput( resample->GetOutput() );
  writer->SetInput( caster->GetOutput()   );
  writer->Update();


  typedef itk::SquaredDifferenceImageFilter< 
                                  FixedImageType, 
                                  FixedImageType, 
                                  OutputImageType > DifferenceFilterType;

  DifferenceFilterType::Pointer difference = DifferenceFilterType::New();

  WriterType::Pointer writer2 = WriterType::New();
  writer2->SetInput( difference->GetOutput() );  
  

  // Compute the difference image between the 
  // fixed and resampled moving image.
  if( argc >= 5 )
    {
    difference->SetInput1( fixedImageReader->GetOutput() );
    difference->SetInput2( resample->GetOutput() );
    writer2->SetFileName( argv[4] );
    writer2->Update();
    }


  // Compute the difference image between the 
  // fixed and moving image before registration.
  if( argc >= 6 )
    {
    writer2->SetFileName( argv[5] );
    difference->SetInput1( fixedImageReader->GetOutput() );
    difference->SetInput2( movingImageReader->GetOutput() );
    writer2->Update();
    }


  return 0;
}

