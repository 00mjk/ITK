/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    wrap_itkAnisotropicDiffusionImageFilter.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkImage.h"
#include "itkAnisotropicDiffusionImageFilter.h"

#ifdef CABLE_CONFIGURATION
#include "wrap_ITKBasicFilters.h"

#define ITK_WRAP_ADIF(x) \
  ITK_WRAP_IMAGE_TO_SAME_IMAGE(AnisotropicDiffusionImageFilter, x);

ITK_WRAP_CONFIG_GROUP(itkAnisotropicDiffusionImageFilter);
ITK_WRAP_DEFINE_IMAGE_TYPES();

ITK_WRAP_ADIF(F2);
ITK_WRAP_ADIF(F3);
ITK_WRAP_ADIF(US2);
ITK_WRAP_ADIF(US3);

#endif
