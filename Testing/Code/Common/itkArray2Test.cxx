/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkArray2Test.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <iostream>

#include "itkArray.h"

// Explicit instantiation to make sure all methods are compiled.
template class itk::Array<float>;



int main(void)
{

  typedef itk::Array<float>  FloatArrayType;
  typedef itk::Array<double> DoubleArrayType;

  FloatArrayType  fa(10);
  DoubleArrayType da(10);

  return EXIT_SUCCESS;

}
