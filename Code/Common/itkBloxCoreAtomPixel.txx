/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBloxCoreAtomPixel.txx
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
#ifndef __itkBloxCoreAtomPixel_txx
#define __itkBloxCoreAtomPixel_txx

#include "itkBloxCoreAtomPixel.h"

namespace itk
{

template <unsigned int NDimensions>
BloxCoreAtomPixel<NDimensions>
::BloxCoreAtomPixel()
{
  m_Eigenvalues.fill(0.0);
  m_Eigenvectors.fill(0.0);
  
  m_MeanCoreAtomDiameter = 0;
}

template <unsigned int NDimensions>
BloxCoreAtomPixel<NDimensions>
::~BloxCoreAtomPixel()
{
  // The default destructor walks the pixel and deletes all bloxitems
}

template <unsigned int NDimensions>
double 
BloxCoreAtomPixel<NDimensions>
::CalcMeanCoreAtomDiameter()
{
  // Returns a mean of 0 if there are no core atoms present
  if( this->empty() )
  {
    return 0;
  }

  unsigned long int numCoreAtoms = 0;
  m_MeanCoreAtomDiameter = 0;

  // The iterator for accessing linked list info
  itk::BloxCoreAtomPixel<NDimensions>::iterator bpiterator;
  
    // Walk through all of the items at the pixel
  for (bpiterator = this->begin(); bpiterator != this->end(); ++bpiterator)
    {
    // Get the pointer of the core atom
    TCoreAtomItemType* pCoreAtom = *bpiterator;

    m_MeanCoreAtomDiameter += pCoreAtom->GetDiameter();
    
    numCoreAtoms++;
    }

  if(numCoreAtoms>0) // Check for /0 to be safe
    m_MeanCoreAtomDiameter /= numCoreAtoms;
  else
    m_MeanCoreAtomDiameter = 0;

  return m_MeanCoreAtomDiameter;
}

template <unsigned int NDimensions>
bool
BloxCoreAtomPixel<NDimensions>
::DoCoreAtomEigenanalysis()
{
  if( this->empty() )
  {
    std::cout << "No core atoms in blox\n";
    return false;
  }

  // The iterator for accessing linked list info
  itk::BloxCoreAtomPixel<NDimensions>::iterator bpiterator;

  // The CMatrix - this is the matrix that we do eigen analysis on
  vnl_matrix_fixed<double, NDimensions, NDimensions> cMatrix;

  // Initialize the CMatrix to 0
  cMatrix.fill(0);

  // The number of items stored in the pixel
  unsigned long int numItems = 0;

  // Walk through all of the items at the pixel
  for (bpiterator = this->begin(); bpiterator != this->end(); ++bpiterator)
    {
    // Get the pointer of the core atom
    TCoreAtomItemType* pCoreAtom = *bpiterator;

    // Get the boundary points
    TBPItemType* pBPOne = pCoreAtom->GetBoundaryPointA();
    TBPItemType* pBPTwo = pCoreAtom->GetBoundaryPointB();
    
    // Get the physical positions of the two boundary points
    TVectorType P1;
    P1 = pBPOne->GetPhysicalPosition().Get_vnl_vector();
    
    TVectorType P2;
    P2 = pBPTwo->GetPhysicalPosition().Get_vnl_vector();

    // Figure out the "C" vector of the core atom
    TVectorType cVector = P2 - P1;
    cVector.normalize();

    // Now, add to the cMatrix
    for(int r = 0; r < NDimensions; r++) // row loop
      {
      for(int c = 0; c < NDimensions; c++) // column loop
        {
        cMatrix(r,c) += cVector[c]*cVector[r];
        } // end column loop
      } // end row loop

    numItems++;

    } // end walk all of the items in the pixel

  // Divide through by the number of items
  cMatrix /= numItems;

  // Create an identity matrix of size n
  vnl_matrix_fixed<double, NDimensions, NDimensions> identMatrix;
  identMatrix.fill(0);

  // Set the diagonal to 1
  for(int n = 0; n < NDimensions; n++) // row loop
    {
    identMatrix(n,n) = 1.0;
    } // end row loop

  // Do eigen analysis
  vnl_generalized_eigensystem* pEigenSys = new vnl_generalized_eigensystem(cMatrix, identMatrix);

  // Now, store the results
  
  // First the eigenvectors
  m_Eigenvectors = pEigenSys->V;

  // Now the eigenvalues (stored as a vector to save space)
  for(int i = 0; i < NDimensions; i++)
    {
    m_Eigenvalues[i] = pEigenSys->D(i,i);
    // Print the eigen values
    std::cout << "Eigenvalue " << i << "=" << m_Eigenvalues[i] << "\n";
    }

  delete pEigenSys;
  return true;
}

} // end namespace itk

#endif
