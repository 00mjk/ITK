// this file defines the RegistrationExamples for the test driver
// and all it expects is that you have a function called RegisterTests
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif
#include <iostream>
#include "vnl/vnl_sample.h"
#include "itkTestMain.h" 


void RegisterTests()
{
  vnl_sample_reseed(8775070);
  REGISTER_TEST(ImageRegistration1Test);
  REGISTER_TEST(ImageRegistration2Test);
  REGISTER_TEST(ImageRegistration3Test);
  REGISTER_TEST(ImageRegistration4Test);
  REGISTER_TEST(ImageRegistration5Test);
  REGISTER_TEST(ImageRegistration6Test);
  REGISTER_TEST(ImageRegistration7Test);
}

#undef main
#define main  ImageRegistration1Test
#undef CommandIterationUpdate
#define CommandIterationUpdate CommandIterationUpdate3
#include "ImageRegistration1.cxx"

#undef main
#define main  ImageRegistration2Test
#undef CommandIterationUpdate
#define CommandIterationUpdate CommandIterationUpdate4
#include "ImageRegistration2.cxx"

#undef main
#define main  ImageRegistration3Test
#undef CommandIterationUpdate
#define CommandIterationUpdate CommandIterationUpdate5
#include "ImageRegistration3.cxx"

#undef main
#define main  ImageRegistration4Test
#undef CommandIterationUpdate
#define CommandIterationUpdate CommandIterationUpdate6
#include "ImageRegistration4.cxx"

#undef main
#define main  ImageRegistration5Test
#undef CommandIterationUpdate
#define CommandIterationUpdate CommandIterationUpdate7
#include "ImageRegistration5.cxx"

#undef main
#define main  ImageRegistration6Test
#undef CommandIterationUpdate
#define CommandIterationUpdate CommandIterationUpdate8
#include "ImageRegistration6.cxx"

#undef main
#define main  ImageRegistration7Test
#undef CommandIterationUpdate
#define CommandIterationUpdate CommandIterationUpdate9
#include "ImageRegistration7.cxx"



