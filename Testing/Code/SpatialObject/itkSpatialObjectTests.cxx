// this file defines the itkStatisticsTest for the test driver
// and all it expects is that you have a function called RegisterTests
#include <iostream>
#include "itkTestMain.h" 


void RegisterTests()
{
  REGISTER_TEST(itkImageSpatialObjectTest);
  REGISTER_TEST(itkTubeSpatialObjectTest);
}
