/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageMomentsCalculator.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkImageMomentsCalculator_txx
#define _itkImageMomentsCalculator_txx
#include "itkImageMomentsCalculator.h"

#include "vnl/algo/vnl_real_eigensystem.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{ 


class ITK_EXPORT InvalidImageMomentsError : public ExceptionObject
{
public:
  /*
   * Constructor. Needed to ensure the exception object can be copied.
   */
  InvalidImageMomentsError(const char *file, unsigned int lineNumber) : ExceptionObject(file, lineNumber) { this->SetDescription("No valid image moments are availble.");}

  /*
   * Constructor. Needed to ensure the exception object can be copied.
   */
  InvalidImageMomentsError(const std::string& file, unsigned int lineNumber) : ExceptionObject(file, lineNumber) { this->SetDescription("No valid image moments are availble.");}  
  
  itkTypeMacro(InvalidImageMomentsError, ExceptionObject);
};

  
//----------------------------------------------------------------------
// Construct without computing moments
template<class TImage>
ImageMomentsCalculator<TImage>::ImageMomentsCalculator(void) 
{
  m_Valid = false;
  m_Image = NULL;
}

//----------------------------------------------------------------------
// Destructor
template<class TImage>
ImageMomentsCalculator<TImage>::
~ImageMomentsCalculator()
{
}

template<class TInputImage>
void
ImageMomentsCalculator<TInputImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Image: " << m_Image.GetPointer() << std::endl;
}

//----------------------------------------------------------------------
// Compute moments for a new or modified image
template<class TImage>
void
ImageMomentsCalculator<TImage>::
Compute()
{
  m_M0 = 0.0;
  m_M1.Fill( 0.0 );
  m_M2.Fill( 0.0 );
  m_Cg.Fill( 0.0 );
  m_Cm.Fill( 0.0 );

  typedef typename ImageType::IndexType IndexType;

  if( !m_Image ) 
    {
    return;
    }
    
  ImageRegionConstIteratorWithIndex< ImageType > it( m_Image,
                                                     m_Image->GetRequestedRegion() ); 

  while( !it.IsAtEnd() )
    {
    double value = it.Value();
    
    IndexType indexPosition = it.GetIndex();
    Point<double, ImageDimension> physicalPosition;

    m_M0 += value;

    for(unsigned int i=0; i<ImageDimension; i++)
      {
      m_M1[i] += static_cast<double>( indexPosition[i] ) * value; 
      for(unsigned int j=0; j<ImageDimension; j++)
        {
        double weight = value * static_cast<double>( indexPosition[i] ) * 
          static_cast<double>( indexPosition[j] );
        m_M2[i][j] += weight;
        }
      }

    m_Image->TransformIndexToPhysicalPoint(indexPosition, physicalPosition);  
    
    for(unsigned int i=0; i<ImageDimension; i++)
      {
      m_Cg[i] += physicalPosition[i] * value; 
      for(unsigned int j=0; j<ImageDimension; j++)
        {
        double weight = value * physicalPosition[i] * physicalPosition[j];
        m_Cm[i][j] += weight;
        }

      }

    ++it;
    }

  // Throw an error if the total mass is zero
  if ( m_M0 == 0.0 )
    throw InvalidImageMomentsError(__FILE__, __LINE__);

  // Normalize using the total mass
  for(unsigned int i=0; i<ImageDimension; i++)
    {
    m_Cg[i] /= m_M0;
    m_M1[i] /= m_M0;
    for(unsigned int j=0; j<ImageDimension; j++)
      {
      m_M2[i][j] /= m_M0;
      m_Cm[i][j] /= m_M0;
      }
    }

  // Center the second order moments
  for(unsigned int i=0; i<ImageDimension; i++)
    {
    for(unsigned int j=0; j<ImageDimension; j++)
      {
      m_M2[i][j] -= m_M1[i] * m_M1[j];
      m_Cm[i][j] -= m_Cg[i] * m_Cg[j];
      }
    }

  // Compute principal moments and axes
  vnl_symmetric_eigensystem<double> eigen( m_Cm.GetVnlMatrix() );
  vnl_diag_matrix<double> pm = eigen.D;
  for(unsigned int i=0; i<ImageDimension; i++)
    {
    m_Pm[i] = pm(i,i) * m_M0;
    }
  m_Pa = eigen.V.transpose();

  // Add a final reflection if needed for a proper rotation,
  // by multiplying the last row by the determinant
  vnl_real_eigensystem eigenrot( m_Pa.GetVnlMatrix() );
  vnl_diag_matrix< vcl_complex<double> > eigenval = eigenrot.D;
  vcl_complex<double> det( 1.0, 0.0 );

  for(unsigned int i=0; i<ImageDimension; i++)
    {
    det *= eigenval( i, i );
    }

  for(unsigned int i=0; i<ImageDimension; i++)
    {
    m_Pa[ ImageDimension-1 ][i] *= std::real( det );
    }
  
  /* Remember that the moments are valid */
  m_Valid = 1;

}


//---------------------------------------------------------------------
// Get sum of intensities
template<class TImage>
typename ImageMomentsCalculator<TImage>::ScalarType
ImageMomentsCalculator<TImage>::
GetTotalMass() const
{
  if (!m_Valid)        throw InvalidImageMomentsError(__FILE__, __LINE__);
  return m_M0;
}

//--------------------------------------------------------------------
// Get first moments about origin, in index coordinates
template<class TImage>
typename ImageMomentsCalculator<TImage>::VectorType
ImageMomentsCalculator<TImage>::
GetFirstMoments() const
{
  if (!m_Valid)        throw InvalidImageMomentsError(__FILE__, __LINE__);
  return m_M1;
}

//--------------------------------------------------------------------
// Get second moments about origin, in index coordinates
template<class TImage>
typename ImageMomentsCalculator<TImage>::MatrixType
ImageMomentsCalculator<TImage>::
GetSecondMoments() const
{
  if (!m_Valid)        throw InvalidImageMomentsError(__FILE__, __LINE__);
  return m_M2;
}

//--------------------------------------------------------------------
// Get center of gravity, in physical coordinates
template<class TImage>
typename ImageMomentsCalculator<TImage>::VectorType
ImageMomentsCalculator<TImage>::
GetCenterOfGravity() const
{
  if (!m_Valid)        throw InvalidImageMomentsError(__FILE__, __LINE__);
  return m_Cg;
}

//--------------------------------------------------------------------
// Get second central moments, in physical coordinates
template<class TImage>
typename ImageMomentsCalculator<TImage>::MatrixType
ImageMomentsCalculator<TImage>::
GetCentralMoments() const
{
  if (!m_Valid)        throw InvalidImageMomentsError(__FILE__, __LINE__);
  return m_Cm;
}

//--------------------------------------------------------------------
// Get principal moments, in physical coordinates
template<class TImage>
typename ImageMomentsCalculator<TImage>::VectorType
ImageMomentsCalculator<TImage>::
GetPrincipalMoments() const
{
  if (!m_Valid)        throw InvalidImageMomentsError(__FILE__, __LINE__);
  return m_Pm;
}



//--------------------------------------------------------------------
// Get principal axes, in physical coordinates
template<class TImage>
typename ImageMomentsCalculator<TImage>::MatrixType
ImageMomentsCalculator<TImage>::
GetPrincipalAxes() const
{
  if (!m_Valid)        throw InvalidImageMomentsError(__FILE__, __LINE__);
  return m_Pa;
}



//--------------------------------------------------------------------
// Get principal axes to physical axes transform
template<class TImage>
typename ImageMomentsCalculator<TImage>::AffineTransformPointer
ImageMomentsCalculator<TImage>::
GetPrincipalAxesToPhysicalAxesTransform(void) const
{
  typename AffineTransformType::MatrixType matrix;
  typename AffineTransformType::OffsetType offset;
  for (unsigned int i = 0; i < ImageDimension; i++) 
    {
    offset[i]  = m_Cg [i];
    for (unsigned int j = 0; j < ImageDimension; j++)
      {
      matrix[j][i] = m_Pa[i][j];    // Note the transposition
      }
    }

  AffineTransformPointer result = AffineTransformType::New();
    
  result->SetMatrix(matrix);
  result->SetOffset(offset);

  return result;
}


//--------------------------------------------------------------------
// Get physical axes to principal axes transform

template<class TImage>
typename ImageMomentsCalculator<TImage>::AffineTransformPointer
ImageMomentsCalculator<TImage>::
GetPhysicalAxesToPrincipalAxesTransform(void) const
{
  typename AffineTransformType::MatrixType matrix;
  typename AffineTransformType::OffsetType offset;
  for (unsigned int i = 0; i < ImageDimension; i++) 
    {
    offset[i]    = m_Cg [i];
    for (unsigned int j = 0; j < ImageDimension; j++)
      {
      matrix[j][i] = m_Pa[i][j];    // Note the transposition
      }
    }

  AffineTransformPointer result = AffineTransformType::New();
  result->SetMatrix(matrix);
  result->SetOffset(offset);

  return result->Inverse();
}

} // end namespace itk



#endif
