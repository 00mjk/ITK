/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkRegularStepGradientDescentOptimizerTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <itkRegularStepGradientDescentOptimizer.h>
#include <vnl/vnl_math.h>

/** 
 *  The objectif function is the quadratic form:
 *
 *  1/2 x^T A x - b^T x
 *
 *  Where A is a matrix and b is a vector
 *  The system in this example is:
 *
 *     | 3  2 ||x|   | 2|   |0|
 *     | 2  6 ||y| + |-8| = |0|
 *
 *
 *   the solution is the vector | 2 -2 |
 *
 */ 
class myCostFunction : public itk::SingleValuedCostFunction 
{
public:

  typedef myCostFunction                     Self;
  typedef itk::SingleValuedCostFunction      Superclass;
  typedef itk::SmartPointer<Self>            Pointer;
  typedef itk::SmartPointer<const Self>      ConstPointer;
  itkNewMacro( Self );

  enum { SpaceDimension=2 };
  
  typedef Superclass::ParametersType      ParametersType;
  typedef Superclass::DerivativeType      DerivativeType;
  typedef Superclass::MeasureType         MeasureType ;


  myCostFunction() 
  {
  }


  MeasureType  GetValue( const ParametersType & parameters ) const
  { 
    
    this->SetParameters( parameters );

    double x = parameters[0];
    double y = parameters[1];

    std::cout << "GetValue( " ;
    std::cout << x << " ";
    std::cout << y << ") = ";

    MeasureType measure = 0.5*(3*x*x+4*x*y+6*y*y) - 2*x + 8*y;

    std::cout << measure << std::endl; 
    return measure;

  }

  DerivativeType  GetDerivative( const ParametersType & parameters ) const
  {

    this->SetParameters( parameters );

    double x = parameters[0];
    double y = parameters[1];

    std::cout << "GetDerivative( " ;
    std::cout << x << " ";
    std::cout << y << ") = ";

    DerivativeType derivative( SpaceDimension ); 
    derivative[0] = 3 * x + 2 * y -2;
    derivative[1] = 2 * x + 6 * y +8;

    std::cout << derivative << std::endl;

    return derivative;
  }

 
  void GetValueAndDerivative( const ParametersType & parameters,
    MeasureType& value, DerivativeType& deriv ) const
  {
    value = this->GetValue( parameters );
    deriv = this->GetDerivative( parameters );
  }



  unsigned int GetNumberOfParameters(void) const
    {
    return SpaceDimension;
    }



private:


};



int main() 
{
  std::cout << "RegularStepGradientDescentOptimizer Test ";
  std::cout << std::endl << std::endl;

  typedef  itk::RegularStepGradientDescentOptimizer  OptimizerType;

  typedef  OptimizerType::ScalesType            ScalesType;
  
  
  // Declaration of a itkOptimizer
  OptimizerType::Pointer  itkOptimizer = OptimizerType::New();


  // Declaration of the CostFunction 
  myCostFunction::Pointer costFunction = myCostFunction::New();


  itkOptimizer->SetCostFunction( costFunction.GetPointer() );

  
  typedef myCostFunction::ParametersType    ParametersType;


  const unsigned int spaceDimension = 
                      costFunction->GetNumberOfParameters();

  // We start not so far from  | 2 -2 |
  ParametersType  initialPosition( spaceDimension );
  initialPosition[0] =  100;
  initialPosition[1] = -100;
  
  ScalesType    parametersScale( spaceDimension );
  parametersScale[0] = 1.0;
  parametersScale[1] = 1.0;

  itkOptimizer->MinimizeOn();
  itkOptimizer->SetScales( parametersScale );
  itkOptimizer->SetGradientMagnitudeTolerance( 1e-6 );
  itkOptimizer->SetMaximumStepLength( 30.0 );
  itkOptimizer->SetMinimumStepLength( 1e-6 );
  itkOptimizer->SetNumberOfIterations( 900 );

  itkOptimizer->SetInitialPosition( initialPosition );

  try 
    {
    itkOptimizer->StartOptimization();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << "Exception thrown ! " << std::endl;
    std::cout << "An error ocurred during Optimization" << std::endl;
    std::cout << "Location    = " << e.GetLocation()    << std::endl;
    std::cout << "Description = " << e.GetDescription() << std::endl;
    return EXIT_FAILURE;
    }


  ParametersType finalPosition = itkOptimizer->GetCurrentPosition();
  std::cout << "Solution        = (";
  std::cout << finalPosition[0] << "," ;
  std::cout << finalPosition[1] << ")" << std::endl;  

  //
  // check results to see if it is within range
  //
  bool pass = true;
  double trueParameters[2] = { 2, -2 };
  for( unsigned int j = 0; j < 2; j++ )
    {
    if( vnl_math_abs( finalPosition[j] - trueParameters[j] ) > 0.01 )
      pass = false;
    }

  if( !pass )
    {
    std::cout << "Test failed." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;


}



