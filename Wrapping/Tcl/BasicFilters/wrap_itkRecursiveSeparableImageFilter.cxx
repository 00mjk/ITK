/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    wrap_itkRecursiveSeparableImageFilter.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkRecursiveSeparableImageFilter.h"
#include "itkImage.h"

#ifdef CABLE_CONFIGURATION
#include "wrap_ITKBasicFilters.h"

#define ITK_WRAP_RSIF(x) \
  ITK_WRAP_IMAGE_TO_SAME_IMAGE(RecursiveSeparableImageFilter, x);

ITK_WRAP_CONFIG_GROUP(itkRecursiveSeparableImageFilter);
ITK_WRAP_DEFINE_IMAGE_TYPES();

ITK_WRAP_RSIF(F2);
ITK_WRAP_RSIF(F3);
ITK_WRAP_RSIF(US2);
ITK_WRAP_RSIF(US3);

#endif
