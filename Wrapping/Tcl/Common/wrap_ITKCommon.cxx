/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    wrap_ITKCommon.cxx
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
#include "wrap_ITKCommon.h"
namespace _cable_
{
  const char* const package = ITK_WRAP_PACKAGE;
  const char* const package_version = ITK_WRAP_PACKAGE_VERSION;
  const char* const groups[] =
  {
    ITK_WRAP_GROUP(ITKUtils),
    ITK_WRAP_GROUP(ITKBase),
    ITK_WRAP_GROUP(ITKFilterBase),
    ITK_WRAP_GROUP(ITKRegions),
    ITK_WRAP_GROUP(itkEventObject),
    ITK_WRAP_GROUP(itkImage)
  };
}
#endif
