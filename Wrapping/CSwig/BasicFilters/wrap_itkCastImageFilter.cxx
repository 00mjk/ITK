/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    wrap_itkCastImageFilter.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkCastImageFilter.h"
#include "itkImage.h"

#ifdef CABLE_CONFIGURATION
#include "itkCSwigMacros.h"
#include "itkCSwigImages.h"

namespace _cable_
{
  const char* const group = ITK_WRAP_GROUP(itkCastImageFilter);
  namespace wrappers
  {
    ITK_WRAP_OBJECT2_WITH_SUPERCLASS(CastImageFilter, image::F2, image::US2, 
                                     itkCastImageFilterF2US2);
    ITK_WRAP_OBJECT2_WITH_SUPERCLASS(CastImageFilter, image::US2, image::F2, 
                                     itkCastImageFilterUS2F2);
    ITK_WRAP_OBJECT2_WITH_SUPERCLASS(CastImageFilter, image::F2, image::F2, 
                                     itkCastImageFilterF2F2);
    ITK_WRAP_OBJECT2_WITH_SUPERCLASS(CastImageFilter, image::US2, image::US2, 
                                     itkCastImageFilterUS2US2);
    ITK_WRAP_OBJECT2_WITH_SUPERCLASS(CastImageFilter, image::F3, image::US3, 
                                     itkCastImageFilterF3US3);
    ITK_WRAP_OBJECT2_WITH_SUPERCLASS(CastImageFilter, image::F3, image::F3, 
                                     itkCastImageFilterF3F3);
    ITK_WRAP_OBJECT2_WITH_SUPERCLASS(CastImageFilter, image::US3, image::US3, 
                                     itkCastImageFilterUS3US3);
  }
}
#endif
