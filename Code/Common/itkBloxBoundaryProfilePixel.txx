/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBloxBoundaryProfilePixel.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBloxBoundaryProfilePixel_txx
#define __itkBloxBoundaryProfilePixel_txx

#include "itkBloxBoundaryProfilePixel.h"

namespace itk
{

template <unsigned int NDimensions>
BloxBoundaryProfilePixel<NDimensions>
::BloxBoundaryProfilePixel()
{

}

template <unsigned int NDimensions>
BloxBoundaryProfilePixel<NDimensions>
::~BloxBoundaryProfilePixel()
{
  // The default destructor walks the pixel and deletes all bloxitems
}

} // end namespace itk

#endif
