/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNthElementDataAccessor.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.

=========================================================================*/
#ifndef __itkNthElementDataAccessor_h
#define __itkNthElementDataAccessor_h



namespace itk
{

/**
 * \class NthElementDataAccessor
 * \brief Give access to the N-th of a Container type 
 *
 * This class is intended to be used as parameter of 
 * an ImageAdaptor to make a  Container appears as being
 * of scalar type T, showing only the N-th component.
 *
 * This class is templated over the container type.
 * Any container type that provides a method:
 * operator[]( unsigned int ) can be used here,
 * for example: itkPoint, itkVector, itkVectorContainer,
 *              and std::vector.
 *
 * For performance, no bound checking is performed during
 * access to the n-th element.
 *
 * \sa ImageAdaptor
 * \sa DataAccessor
 *
 */

template < class T, class TContainer >
class ITK_EXPORT NthElementDataAccessor
{
public:
 /**
   * Standard "Self" typedef.
   */
  typedef   NthElementDataAccessor        Self;

 /** 
   * External typedef. It defines the external aspect
   * that this class will exhibit
   */
  typedef T ExternalType;

  /** 
   * Internal typedef. It defines the internal real
   * representation of data
   */
  typedef   TContainer    InternalType;


  /** 
   * Write access to the NthElement component
   */
  inline void Set( InternalType & output, const ExternalType & input ) const 
    { output[m_ElementNumber] =  input; }


  /** 
   * Read access to the NthElement component
   */
  inline ExternalType Get( const InternalType & input ) const
    { return static_cast<ExternalType>( input[m_ElementNumber] ); }

  /** 
   * Get the element number to access in the container
   */
  unsigned int GetElementNumber(void) const
  { return m_ElementNumber; }

  /** 
   * Set the element number to access in the container
   */
  void SetElementNumber( unsigned int nth )
  { m_ElementNumber = nth; }


private:
  // Identifier of the N-th element to be accessed
  unsigned int    m_ElementNumber;

};

  
}  // end namespace itk

#endif

