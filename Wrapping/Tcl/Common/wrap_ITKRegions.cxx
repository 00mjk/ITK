/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    wrap_ITKRegions.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkRegion.h"
#include "itkImageRegion.h"
#include "itkMeshRegion.h"

#ifdef CABLE_CONFIGURATION
#include "wrap_ITKCommon.h"

ITK_WRAP_CONFIG_GROUP(ITKRegions);
ITK_WRAP_CLASS(Region);
ITK_WRAP_CLASS(MeshRegion);
ITK_WRAP_CLASS_TEMPLATE_1(ImageRegion2, ImageRegion<2>);
ITK_WRAP_CLASS_TEMPLATE_1(ImageRegion3, ImageRegion<3>);

#endif
