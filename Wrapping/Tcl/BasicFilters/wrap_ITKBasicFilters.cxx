/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    wrap_ITKBasicFilters.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifdef CABLE_CONFIGURATION
#include "wrap_ITKBasicFilters.h"
namespace _cable_
{
  const char* const package = ITK_WRAP_PACKAGE;
  const char* const package_version = ITK_WRAP_PACKAGE_VERSION;
  const char* const groups[] =
  {
    ITK_WRAP_GROUP(itkAnisotropicDiffusionImageFilter),
    ITK_WRAP_GROUP(itkBinaryThresholdImageFilter),
    ITK_WRAP_GROUP(itkCannyEdgeDetectionImageFilter),
    ITK_WRAP_GROUP(itkCastImageFilter),
    ITK_WRAP_GROUP(itkConfidenceConnectedImageFilter),
    ITK_WRAP_GROUP(itkConnectedThresholdImageFilter),
    ITK_WRAP_GROUP(itkCurvatureAnisotropicDiffusionImageFilter),
    ITK_WRAP_GROUP(itkGradientAnisotropicDiffusionImageFilter),
    ITK_WRAP_GROUP(itkLaplacianImageFilter),
    ITK_WRAP_GROUP(itkMinimumMaximumImageCalculator),
    ITK_WRAP_GROUP(itkNaryAddImageFilter),
    ITK_WRAP_GROUP(itkRandomImageSource),
    ITK_WRAP_GROUP(itkRecursiveGaussianImageFilter),
    ITK_WRAP_GROUP(itkRecursiveSeparableImageFilter),
    ITK_WRAP_GROUP(itkResampleImageFilter),
    ITK_WRAP_GROUP(itkRescaleIntensityImageFilter),
    ITK_WRAP_GROUP(itkSobelEdgeDetectionImageFilter),
    ITK_WRAP_GROUP(itkTernaryMagnitudeImageFilter)
  };
}
#endif
