/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    itkMetaDataDictionary.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) 2002 Insight Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

Portions of this code are covered under the VTK copyright.
See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef MetaDataDictionary_h_h
#define MetaDataDictionary_h_h

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <map>
#include <string>
#include "itkMetaDataObjectBase.h"

namespace itk
{
  /**
   * \author Hans J. Johnson
   * The MetaDataDictionary, along with the MetaDataObject derived template
   * classes, is designed to provide a mechanism for storing a collection of
   * arbitrary data types. The main motivation for such a collection is to
   * associate arbitrary data elements with itk DataObjects.
   */
  class ITKCommon_EXPORT MetaDataDictionary
    :private std::map<std::string, MetaDataObjectBase::Pointer>
    {
      public:
        typedef MetaDataDictionary Self;
        typedef std::map<std::string, itk::MetaDataObjectBase::Pointer>  Superclass;
        typedef Self * Pointer;
        typedef const Self * ConstPointer;
        /**
         * Defines the default behavior for printing out this element
         * \param os An output stream
         */
        virtual void Print(std::ostream& os) const;
        //NOTE: Currently this is blank, but it is here to allow the
        //implementation of future functionality that may become apparent
        //at a latter date.
        //Right now, I can not think of any neccessary functionality that is
        //not available from the std::map class.
        virtual ~MetaDataDictionary();

        // Implement map's api. On some Micorsoft compilers, stl containers
        // cannot be exported. This causes problems when building DLL's.
        // Here we inherit privately from std::map and provide a simple
        // API. The implementation will be in the DLL.
        MetaDataObjectBase::Pointer &operator [](const std::string &);
        bool HasKey (const std::string &);
    };
}
#endif// MetaDataDictionary_h_h

