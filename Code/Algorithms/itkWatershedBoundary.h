/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkWatershedBoundary.h
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
#ifndef __itkWatershedBoundary_h
#define __itkWatershedBoundary_h

#include <list>
#include <vector>
#include "itkImage.h"
#include "itkObjectFactory.h"
#include "itkDataObject.h"
#include "itkProcessObject.h"
#include "itk_hash_map.h"

namespace itk
{
namespace watershed
{
/** \class Boundary
 * \par
 * A data object for used by watershed segmentation process objects in
 * streaming applications. A ``boundary'' represents the single-pixel wide
 * surface of an image chunk. This class is used to store information needed to
 * resolve processing at chunk  boundaries after data streaming of the
 * watershed segmentation algorithm.
 * 
 * \par
 * This is an itkDataObject.  It contains a matrix of ``faces'' of an
 * N-dimensional hypercube. A chunk of a volume with /f$ N /f$ dimensions
 * has /f$ 2N /f$ faces of dimension /f$ N-1 /f$. Some examples: A
 * 2-dimensional image has 4 faces that are lines.  A 3-dimensional image has 6 
 * faces that are planes.  A 4-dimensional image has 8 faces which are
 * cubes. Faces are indexed as /f$ N /f$ sets of pairs /f$ (low, high) /f$.
 *
 * \sa WatershedSegmenter
 * \sa WatershedBoundaryResolver
 * \ingroup WatershedSegmentation */
template <class TScalarType, unsigned int TDimension>
class Boundary : public DataObject
{
public:
  /** The dimensionality of this boundary.  For example, if the boundary
   * of a set of planes, it has dimensionality 2.  If the boundary is
   * a set of lines, it has dimensionality 1.  Dimensionality is one less
   * than the image chunks from which the boundary is derived.   */
  enum {Dimension = TDimension};

  /** A pair of values used to index into the boundary data structure.
   * The IndexType.first is the dimension of the face and IndexType.second is a
   * binary value 0 or 1 indicating the LOW face or the HIGH face,
   * respectively.    */
  typedef std::pair<unsigned, unsigned> IndexType;
  typedef typename Image<unsigned long, TDimension>::IndexType ImageIndexType;
  typedef TScalarType ScalarType;

  /** Data type stored at each pixel in a face.   */
  struct face_pixel_t
  {
    /**Index of the direction of watershed flow through this pixel.
     * A negative value indicates that the flow does not move out
     * of the region.  A positive value is the index into the
     * pixel neighborhood of the facing chunk boundary into which
     * flow moves.
     *
     * Note that the range of values of the index depends on the
     * the connectivity used by the watershed segmentation algorithm.
     * If the WS algorithm uses city-block style connectivity (4-connectivity
     * in 2D, 6-connectivity in 3D, etc) this could actually be a boolean
     * value indicating inward or outward flow since there is only one
     * valid neighbor to reference.  For extensibility to other
     * connectivities, this flow value can be used to index a number of
     * different neighbors.     */
    short flow;

    /** The label associated with this pixel.     */
    unsigned long label;
  };

  /**    */
  struct flat_region_t
  {
    /** Indicies into the associated Face containing boundary pixels.  These
     * give access to spatial information, label and flow associated with
     * this boundary pixel connection.     */
    std::list<unsigned long> offset_list;

    /** The value of the lowest point (indicating the steepest descent) along
     * the boundary of the flat region of which this pixel is a member.     */
    ScalarType bounds_min;

    /** The label associated with the lowest point
     * point along this flat region boundary.     */
    unsigned long min_label;

    /** The value of this flat region     */
    ScalarType value;
  };
  
  /** The face data structure.  This is just an itk::Image of face pixel
      types. */
  typedef itk::Image<face_pixel_t, Dimension> face_t;

  /** A hash table holding flat region data structures.   */
  typedef itk::hash_map<unsigned long, flat_region_t,
    itk::hash<unsigned long> > flat_hash_t;
  
  /** Itk typedefs and macros defining smart pointer and type identification.
   */
  typedef Boundary Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  itkNewMacro(Self);
  itkTypeMacro(Boundary, DataObject);

  /** The following averts an internal compiler error on microsoft compilers */
  typedef typename face_t::Pointer FacePointer;

  
  /** Returns the face at the specified index  */
  FacePointer GetFace(const IndexType &idx)
    {      return this->GetFace(idx.first, idx.second);    }

  /** Returns the face at the specified index, where dimension is
   * the number of the axial dimension and highlow is 0 for the LOW
   * face and 1 for the HIGH face.   */ 
  FacePointer GetFace(unsigned dimension, unsigned highlow)
    {
      if (highlow == 0) return m_Faces[dimension].first;
      else return m_Faces[dimension].second;
    }

  void SetFace(FacePointer f, const IndexType &idx)
    { this->SetFace(f, idx.first, idx.second); }
  
  void SetFace(FacePointer f, unsigned dimension, unsigned highlow)
    {
      if (highlow ==0 ) m_Faces[dimension].first = f;
      else m_Faces[dimension].second = f;
      this->Modified();
    }
  
  /** Get/Set the table of flat region connections specified by the index. */
  flat_hash_t *GetFlatHash(const IndexType &idx)
    { return this->GetFlatHash(idx.first, idx.second); }
  flat_hash_t *GetFlatHash(unsigned dimension, unsigned highlow)
    {
      if (highlow == 0) return &(m_FlatHashes[dimension].first);
      else return &(m_FlatHashes[dimension].second);
    }
  void SetFlatHash(flat_hash_t & l, const IndexType &idx)
    { this->SetFlatHash(l, idx.first, idx.second); }  
  void SetFlatHash(flat_hash_t & l, unsigned dimension,
                             unsigned highlow)
    {
      if (highlow ==0 ) m_FlatHashes[dimension].first = l;
      else m_FlatHashes[dimension].second = l;
      this->Modified();
    }

  /** Marks a face in the boundary object as either valid (true) or
   * invalid (false).  A valid face is assumed to be initialized
   * and contain information.  No assumptions are made about an
   * invalid face.   */
  void SetValid(bool & l, const IndexType &idx)
    { this->SetValid(l, idx.first, idx.second); }  
  void SetValid(bool b, unsigned dimension,
                             unsigned highlow)
    {
      if (highlow ==0 ) m_Valid[dimension].first = b;
      else m_Valid[dimension].second = b;
      this->Modified();
    }
  bool GetValid(const IndexType &idx) const
    { return this->GetValid(idx.first, idx.second); }
  bool GetValid(unsigned dimension, unsigned highlow) const
    {
      if (highlow == 0) return m_Valid[dimension].first;
      else return m_Valid[dimension].second;
    }

protected:
  Boundary();
  virtual ~Boundary() {}
  Boundary(const Self&) {}
  void operator=(const Self&) {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** The Nx2 matrix of faces of this boundary.   */
  std::vector<std::pair<FacePointer, FacePointer> > m_Faces;

  /** The Nx2 matrix flat region connections associated with
   * this boundary.   */
  std::vector<std::pair<flat_hash_t, flat_hash_t> >  m_FlatHashes;

  /** The Nx2 matrix of boolean flags indicating which faces contain
   * information.    */
  std::vector<std::pair<bool,bool> >  m_Valid;

  /*@{ Virtual methods required by DataObject interface.   */
  void UpdateOutputInformation();
  bool VerifyRequestedRegion() { return true; }
  void SetRequestedRegionToLargestPossibleRegion () {}
  bool RequestedRegionIsOutsideOfTheBufferedRegion () { return false; }
  void SetRequestedRegion (itk::DataObject *) {}
  //@}
};
}// end namespace watershed
}// end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkWatershedBoundary.txx"
#endif

#endif

