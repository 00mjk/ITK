/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkNumericTraitsTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include <iostream>
#include "itkNumericTraits.h"


template<class T> void CheckTraits(char *name, T)
{
  std::cout << "itk::NumericTraits<" << name << ">" << std::endl;
  std::cout << "\tmin(): " << static_cast<typename itk::NumericTraits<T>::PrintType>(itk::NumericTraits<T>::min()) << std::endl;
  std::cout << "\tmax(): " << static_cast<typename itk::NumericTraits<T>::PrintType>(itk::NumericTraits<T>::max()) << std::endl;
  std::cout << "\tNonpositiveMin(): " << static_cast<typename itk::NumericTraits<T>::PrintType>(itk::NumericTraits<T>::NonpositiveMin()) << std::endl;
  std::cout << "\tis_signed: " << itk::NumericTraits<T>::is_signed << std::endl;
}


int itkNumericTraitsTest(int, char* [] )
{
//  CheckTraits("char", static_cast<char>(0));
  CheckTraits("signed char", static_cast<signed char>(0));
  CheckTraits("unsigned char", static_cast<unsigned char>(0));
  CheckTraits("short", static_cast<short>(0));
  CheckTraits("unsigned short", static_cast<unsigned short>(0));
  CheckTraits("int", static_cast<int>(0));
  CheckTraits("unsigned int", static_cast<unsigned int>(0));
  CheckTraits("long", static_cast<long>(0));
  CheckTraits("unsigned long", static_cast<unsigned long>(0));
  CheckTraits("float", static_cast<float>(0));
  CheckTraits("double", static_cast<double>(0));

  return 0;
}
