/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBloxPixel.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBloxPixel_txx
#define __itkBloxPixel_txx

#include "itkBloxPixel.h"

namespace itk
{

template <typename TItemType>
BloxPixel<TItemType>
::BloxPixel()
{

}

template <typename TItemType>
BloxPixel<TItemType>
::~BloxPixel()
{
  // We need to clean up memory used by linked list entries
  // Walk through all of the elements at the pixel and delete what we find

  //printf("In ~BloxPixel()\n");
  
  this->DeleteListEntries();
}

template <typename TItemType>
BloxPixel<TItemType>
::DeleteListEntries()
{
  //printf("In DeleteListEntries()\n");

  if( !( this->empty() ) )
    {
    // The iterator for accessing linked list info
    BloxPixel::iterator bpiterator;
  
    for (bpiterator = this->begin(); bpiterator != this->end(); ++bpiterator)
      {
      delete (*bpiterator);
      }
    }

    // Empty the linked list
    this->clear();
}

} // end namespace itk

#endif
