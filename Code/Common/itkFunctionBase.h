/*==========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFunctionBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef _itkFunctionBase_h
#define _itkFunctionBase_h

#include "itkObject.h"
#include "itkObjectFactory.h"

namespace itk
{

/**
 * \class FunctionBase
 * \brief Base class for all ITK function objects
 *
 * FunctionBase is the base class for ITK function objects. Specifically,
 * the abstract method Evaluate() maps a point from the input space to a point
 * in the output space.
 *
 * Subclasses must override Evaluate(). 
 * 
 * This class is template over the input (domain) type and 
 * the output (range) type.
 *
 * \ingroup Functions
 * 
 */
template < class TInput, class TOutput >
class ITK_EXPORT FunctionBase : public Object
{
public:
  /** Standard class typedefs. */
  typedef FunctionBase Self;
  typedef Object Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(FunctionBase, Object);

  /** Input type */
  typedef TInput InputType;

  /** Output type */
  typedef TOutput OutputType;

  /** Evaluate at the specified input position */
  virtual OutputType Evaluate( const InputType& input ) const = 0;

protected:
  FunctionBase(){};
  ~FunctionBase(){};

private:
  FunctionBase(const Self& ); //purposely not implemented
  void operator=(const Self& ); //purposely not implemented

};

} // namespace itk

#endif
