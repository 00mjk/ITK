/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageMomentsCalculator.h
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
#ifndef __itkImageMomentsCalculator_h
#define __itkImageMomentsCalculator_h

#include "itkAffineTransform.h"
#include "itkMacro.h"
#include "itkImage.h"

#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_diag_matrix.h"

namespace itk
{

/** \class ImageMomentsCalculator
 * \brief Compute moments of an n-dimensional image.
 *
 * This class provides methods for computing the moments and related
 * properties of a single-echo image.  Computing the (non-central)
 * moments of a large image can easily take a million times longer
 * than computing the various other values derived from them, so we
 * compute the moments only on explicit request, and save their values
 * (in an ImageMomentsCalculator object) for later retrieval by the user.
 *
 * The non-central moments computed by this class are not really 
 * intended for general use and are therefore in index coordinates;
 * that is, we pretend that the index that selects a particular
 * pixel also equals its physical coordinates.  The center of gravity,
 * central moments, principal moments and principal axes are all
 * more generally useful and are computed in the physical coordinates
 * defined by the Origin and Spacing parameters of the image.
 *
 * The methods that return values return the values themselves rather
 * than references because the cost is small compared to the cost of
 * computing the moments and doing so simplifies memory management for
 * the caller.
 *
 * \ingroup Operators
 *
 * \todo It's not yet clear how multi-echo images should be handled here.
 */
template < class TImage >
class ImageMomentsCalculator
{
public:
  /** Standard class typedefs. */
  typedef ImageMomentsCalculator<TImage>   Self;

  /** Extract the dimension of the image. */
  enum { ImageDimension = TImage::ImageDimension };

  /** Standard scalar type within this class. */
  typedef double                       ScalarType;

  /** Standard vector type within this class. */
  typedef Vector<ScalarType,ImageDimension>         VectorType;

  /** Standard matrix type within this class. */
  typedef Matrix<ScalarType,
                 ImageDimension,ImageDimension>   MatrixType;

  /** Standard image type within this class. */
  typedef TImage ImageType;

  /** Standard image type pointer within this class. */
  typedef typename ImageType::Pointer ImagePointer;

  /** Standard affine transform type for this class. */
  typedef AffineTransform<ScalarType, ImageDimension> AffineTransformType;

  /** Standard affine transform type for this class. */
  typedef typename AffineTransformType::Pointer AffineTransformPointer;

  /** Compute moments of a new or modified image.
   * This method computes the moments of the image given as a
   * parameter and stores them in the object.  The values of these
   * moments and related parameters can then be retrieved by using
   * other methods of this object. */
  void ComputeMoments( ImageType * image );

  /** Return the total mass (or zeroth moment) of an image.
   * This method returns the sum of pixel intensities (also known as
   * the zeroth moment or the total mass) of the image whose moments
   * were last computed by this object. */
  ScalarType GetTotalMass();

  /** Return first moments about origin, in index coordinates.
   * This method returns the first moments around the origin of the
   * image whose moments were last computed by this object.  For
   * simplicity, these moments are computed in index coordinates
   * rather than physical coordinates. */
  VectorType GetFirstMoments();

  /** Return second moments about origin, in index coordinates.
   * This method returns the second moments around the origin
   * of the image whose moments were last computed by this object.
   * For simplicity, these moments are computed in index coordinates
   * rather than physical coordinates. */
  MatrixType GetSecondMoments();

  /** Return center of gravity, in physical coordinates.
   * This method returns the center of gravity of the image whose
   * moments were last computed by this object.  The center of
   * gravity is computed in physical coordinates. */
  VectorType GetCenterOfGravity();

  /** Return second central moments, in physical coordinates.
   * This method returns the central second moments of the image
   * whose moments were last computed by this object.  The central
   * moments are computed in physical coordinates. */
  MatrixType GetCentralMoments();

  /** Return principal moments, in physical coordinates.
   * This method returns the principal moments of the image whose
   * moments were last computed by this object.  The moments are
   * returned as a vector, with the principal moments ordered from
   * smallest to largest.  The moments are computed in physical
   * coordinates.   */
  VectorType GetPrincipalMoments();

  /** Return principal axes, in physical coordinates.
   * This method returns the principal axes of the image whose
   * moments were last computed by this object.  The moments are
   * returned as an orthogonal matrix, each row of which corresponds
   * to one principal moment; for example, the principal axis
   * corresponding to the smallest principal moment is the vector
   * m[0], where m is the value returned by this method.  The matrix
   * of principal axes is guaranteed to be a proper rotation; that
   * is, to have determinant +1 and to preserve parity.  (Unless you
   * have foolishly made one or more of the spacing values negative;
   * in that case, _you_ get to figure out the consequences.)  The
   * moments are computed in physical coordinates. */
  MatrixType GetPrincipalAxes();

  /** Get the affine transform from principal axes to physical axes
   * This method returns an affine transform which transforms from
   * the principal axes coordinate system to physical coordinates. */
  AffineTransformPointer GetPrincipalAxesToPhysicalAxesTransform(void) const;

  /** Get the affine transform from physical axes to principal axes
   * This method returns an affine transform which transforms from
   * the physical coordinate system to the principal axes coordinate
   * system. */
  AffineTransformPointer GetPhysicalAxesToPrincipalAxesTransform(void) const;

  /** Construct an ImageMomentsCalculator object.  This method constructs a
   * new ImageMomentsCalculator object that contains no stored moments
   * information; this information can be added later by calling the
   * ComputeMoments method. */
  ImageMomentsCalculator();            // Create w/o summing moments

  /** Compute moments of an image and save in an ImageMomentsCalculator
   * object.  This method constructs a new ImageMomentsCalculator object and
   * stores in it the moments of the image given as argument.  The values of
   * these moments and related parameters can be retrieved by using * other
   * methods of the object constructed.  */
  ImageMomentsCalculator(ImageType * image); // Create and sum image moments

  /** Destroy an ImageMomentsCalculator object. */
  ~ImageMomentsCalculator();

private:
  bool m_valid;                      // Have moments been computed yet?
  ScalarType m_m0;                   // Zeroth moment
  VectorType m_m1;                   // First moments about origin
  MatrixType m_m2;                   // Second moments about origin
  VectorType m_cg;                   // Center of gravity (physical units)
  MatrixType m_cm;                   // Second central moments (physical)
  VectorType m_pm;                   // Principal moments (physical)
  MatrixType m_pa;                   // Principal axes (physical)

};  // class ImageMomentsCalculator

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageMomentsCalculator.txx"
#endif

#endif /* __itkImageMomentsCalculator_h */
