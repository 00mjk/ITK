/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkArray.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkArray_txx
#define _itkArray_txx

#include "itkArray.h"

namespace itk
{



/**
 * Default constructor 
 */
template < typename TValueType >
Array<TValueType >
::Array():vnl_vector<TValueType>()
{
}


/**
 * Constructor with size
 */
template < typename TValueType >
Array<TValueType >
::Array(unsigned int dimension):vnl_vector<TValueType>(dimension)
{
}

template < typename TValueType >
void Array<TValueType >
::SetSize(unsigned int sz)
{
  this->set_size(sz);
}


} // namespace itk

#endif
