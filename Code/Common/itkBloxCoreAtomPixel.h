/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBloxCoreAtomPixel.h
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
#ifndef __itkBloxCoreAtomPixel_h
#define __itkBloxCoreAtomPixel_h

#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/algo/vnl_generalized_eigensystem.h"

#include "itkBloxCoreAtomItem.h"
#include "itkBloxBoundaryPointItem.h"
#include "itkPoint.h"
#include "itkCovariantVector.h"
#include "itkBloxPixel.h"

namespace itk
{

/**
 * \class BloxCoreAtomPixel
 * \brief Holds a linked list of itk::BloxCoreAtomItem's
 *
 * \ingroup ImageObjects 
 * */

template <unsigned int NDimensions>
class BloxCoreAtomPixel : public BloxPixel< BloxCoreAtomItem<NDimensions> >
{

public:

  /**
   * The type of core atom item we process
   * */
  typedef BloxCoreAtomItem<NDimensions> TCoreAtomItemType;

  /**
   * The type of boundary point item we process
   * */
  typedef BloxBoundaryPointItem<NDimensions> TBPItemType;

  /**
   * The type used to store the position of the BoundaryPointItem
   * */
  typedef Point<double, NDimensions> TPositionType;
  
  /**
   * The type of vector used to store the gradient of the BoundaryPointItem
   * */
  typedef CovariantVector<double, NDimensions> TGradientType;

  /**
   * VNL type used in eigenanalysis
   * */
  typedef vnl_vector_fixed<double, NDimensions> TVectorType;

  /*
   * Vector type used to store eigenvalues
   * */
  typedef vnl_vector_fixed<double, NDimensions> TEigenvalueType;

  /*
   * Matrix type used to store eigenvectors
   * */
  typedef vnl_matrix_fixed<double, NDimensions, NDimensions> TEigenvectorType;

  /**
   * Calculate and store the mean of core atom diameters
   * */
  double CalcMeanCoreAtomDiameter();

  /**
   * Perform eigenanalysis on the population of core atoms stored in this pixel
   * */
  bool DoCoreAtomEigenanalysis();

  /**
   * Get the mean core atom diameter
   */
  double GetMeanCoreAtomDiameter() {
    return m_MeanCoreAtomDiameter; }

  /**
   * Get eigenvalues
   */
  TEigenvalueType GetEigenvalues() {
    return m_Eigenvalues; }

  /**
   * Get eigenvectors
   */
  TEigenvectorType GetEigenvectors() {
    return m_Eigenvectors; }

  BloxCoreAtomPixel();
  ~BloxCoreAtomPixel();

private:

  /*
   * The eigenvalues of the core atom population in this pixel
   * These are stored in increasing order of value (not absolute value) from
   * indices 0 to n, where n is the number of dimensions in the source image
   */
  TEigenvalueType m_Eigenvalues;

  /*
   * The eigenvectors of the core atom population in this pixel
   * Each eigenvector is a row? of this matrix
   */
  TEigenvectorType m_Eigenvectors;

  /*
   * Average (arithmetic mean) of core atom diameters stored in this pixel
   */
  double m_MeanCoreAtomDiameter;
};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBloxCoreAtomPixel.txx"
#endif

#endif
