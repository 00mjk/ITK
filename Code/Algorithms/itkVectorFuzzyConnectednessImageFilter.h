/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkVectorFuzzyConnectednessImageFilter.h
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
#ifndef __itkVectorFuzzyConnectednessImageFilter_h
#define __itkVectorFuzzyConnectednessImageFilter_h

#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkVector.h"
#include "itkMatrix.h"
#include <vector>
#include <list>

namespace itk{

/** \class VectorFuzzyConnectednessImageFilter
 * The purpose of our program is to segment an object of interest in vectorial images 
 * using fuzzy connectedness principles. Here is a very brief description of these 
 * principles.
 *
 * There are four basic concepts in fuzzy connectedness: fuzzy affinity [1,2], fuzzy 
 * connectedness [1], scale [2], and relative fuzzy connectedness [3]. These references
 * give a complete theoretical basis and description of the underlying algorithms. For algorithm implementation and efficiency, see [4].
 *
 * Fuzzy affinity is a local fuzzy relation which is defined on the whole image domain.
 * It assigns to every pair of nearby voxels a strength of local hanging togetherness.
 * Two components of fuzzy affinity named homogeneity-feature-based affinity and
 * object-feature-based affinity are devised in a fully vectorial manner. The strength
 * of affinity between any two nearby voxels v1 and v2 is large when v1 and v2 are
 * spatially close, the intensity vectors of voxels in the "vicinity" of v1 and v2 have
 * a high degree of homogeneity, and the intensity vectors are close to an intensity
 * vector expected for the object of interest. (If v1 and v2 are far apart, their affinity
 * is considered to be 0) The extent of "vicinity" is determined by the "scale" value
 * assigned to v1 and v2. 
 *
 * "Scale" is a number assigned to every voxel in the image. It represents the radius of
 * the largest ball centered at the voxel within which the vectorial values of all voxels
 * are homogeneous. In determining affinity between two voxels, all voxels within the ball
 * associated with both voxels are considered. Scale-based fuzzy affinity and fuzzy
 * connectedness can make our algorithms more robust and less sensitive to noise. 
 * 
 * A global fuzzy relation called fuzzy connectedness is defined on the whole image domain
 * which assigns to every pair of voxels a strength of global hanging togetherness. The
 * strength of hanging togetherness (connectedness) between two voxels is the largest of
 * the strengths of all paths between them. The strength of any path is simply the smallest
 * affinity of successive pair of voxels along the path.
 * 
 * The other important concept is relative fuzzy connectedness. An object gets defined in
 * an image because of the presence of other co-objects. All co-objects of importance that
 * are present in an image are let to compete among themselves in having voxels as their
 * members. In this competition, every pair of voxels in the image will have a strength of
 * connectedness in each object. The object in which this strength is highest will claim
 * membership of the voxels. Suppose that there are n objects (note that image background
 * is also an object) in the image and that we specify a seed voxel si in each object.
 * Then any voxel v in the image has a strength of connectedness with respect to each si.
 * Voxel v is considered to belong to that object with respect to whose seed v has the
 * highest strength of connectedness [3]. The method requires some modification when a set
 * of seed voxels is specified for each object. The basic algorithmic tool required in all
 * cases is dynamic programming.
 * 
 * The fuzzy connectedness methods have been utilized on 1000s of images in several
 * applications [6-10].
 *
 * Our program takes a vectorial image as input and produces one binary output. The
 * procedure includes six main steps.
 * 1. Use function Compute_LookupTable () to setup three look-up tables m_ScaleMap,
 *    m_HomogeneityMap and m_ObjectMap in order to facilitate the computation of scale,
 *    homogeneity-based affinity and object-feature-based affinity.
 * 2. Use function Compute_Scale() to compute scale value at every voxel in and keep
 *    them in m_ScaleArray. In our program we set the maximum scale value as 8, this
 *    value could be changed in terms of the input image.
 * 3. Use function Compute_Filter() to compute scale-based filtered vectorial value at
 *    every voxel and keep the value in m_FilterImage, this step is to speed up the
 *    computation of next step.  
 * 4. Use function Compute_Affinity() to compute fuzzy affinity value of each pair of
 *    neighboring voxels and keep them in m_Xaffinity, m_Yaffinity, and m_Zaffinity in
 *    terms of the coordinate direction. At first, we compute homogeneity-based affinity,
 *    them compute object-feature-based affinity, finally we combine them together.
 * 5. Use function FastTracking() to compute global fuzzy connectedness between every
 *    voxel and seed points of foreground (selected object) and compute global fuzzy
 *    connectedness between every voxel and seed points of background ( other objects).
 *    And keep their values in m_ObjectFuzzyScene and m_BackgroundFuzzyScene respectively.
 * 6. In function GenerateDate(), we implement iterative relative fuzzy connectedness
 *    strategy. We let foreground and background to compete between them for having
 *    voxels as their members in an iterative fashion [5].
 *
 * Before running our program, user needs to input some parameters.
 * 
 * 1. Use function SetObjects to set the number of objects of importance in the image.
 * 2. Use function SetSelectedObject to specify the particular object to be segmented.
 * 3. Use function SetObjectsSeed to specify seed points for objects.
 * 4. Use function SetObjectsMean to specify the mean value of the intensity vector for
 *    objects.
 * 5. Use function SetObjectsMatrix to specify the covariance matrix of the intensity
 *    vector for objects.
 *
 *
 *Reference:
 *
 *1. J. K. Udupa and S. Samarasekera, Fuzzy connectedness and object definition: Theory,
 *   algorithms, and applications in image segmentation, Graphical Models Image Processing
 *   58, 1996, 246-261.
 *2. P. K. Saha, J. K. Udupa, and D. Odhner, Scale-based fuzzy connected image 
 *   segmentation: theory, algorithms, and validation, Computer Vision and Image 
 *   Understanding 77, 2000, 145-174.
 *3. P. K.Saha and J. K. Udupa, Relative Fuzzy connectedness among multiple objects:
 *   Theory, algorithms, and applications in image segmentation, Computer Vision and
 *   Image Understanding 82, 2001, 42-56.
 *4. L. G. Nyul and J. K. Udupa, Fuzzy-connected 3D image segmentation at interactive
 *   speeds, Proc. SPIE: Medical Imaging 3979, 2000, 212-223.
 *5. P. K.Saha and J. K. Udupa, Iterative relative fuzzy connectedness and object
 *   definition: Theory, algorithms, and applications in image segmentation, in Proc. Of
 *   IEEE Workshop on Mathematical Methods in Biomedical Image Analysis, 2000, 28-35.
 *6. J. K. Udupa, L. Wei, S. Samarasekera, Y. Miki, M. A. van Buchen, and R. I. Grossman,
 *   Multiple sclerosis lesion quantification using fuzzy connected principles, IEEE 
 *   Trans. Med. Imag. 16(5), 1997, 598-609.
 *7. B. L. Rice and J. K. Udupa, Fuzzy connected clutter-free volume rendering for MR
 *   angiography, Int. J. Imaging System. Tech. 11, 2000, 62-70.
 *8. T. Lei and J. K. Udupa, Artery-Vein Separation via MRA-An Image Processing Approch,
 *   IEEE Trans. Med. Imag. 20(8), 2001, 689-703.
 *9. P. K. Saha, J. K. Udupa, E. F. conant, D. P. Chakraborty, and D. Sullivan, Breast
 *   Tissue density Quantification via Digitized Mammograms, IEEE Trans. Med. Imag.
 *   20(8), 2001, 792-803.
 *10. J. Liu, J. K. Udupa, D. Hackney, and G. Moonis, Brain tumor segmentation in MRI
 *   by using the fuzzy connectedness method, Proc. SPIE: Medical Imaging 4322, 2001,
 *   1455-1465.
 * 
 */
template <class TInputImage, class TOutputImage>
class VectorFuzzyConnectednessImageFilter:
  public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef VectorFuzzyConnectednessImageFilter       Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>   Superclass;
  typedef SmartPointer <Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VectorFuzzyConnectednessImageFilter,ImageToImageFilter);

  /** Extract the image and vector types from the template parameters. */
  typedef typename TInputImage::PixelType InputPixelType;
  typedef typename InputPixelType::VectorType InputVectorType;

  /** Extract the image and vector dimension from the template parameters. */
  enum {ImageDimension = TInputImage::ImageDimension };
  enum {VectorDimension = InputPixelType::VectorDimension};

  /** Vector and matrix related typedefs. */
  typedef   Matrix<double,VectorDimension,VectorDimension>               MatrixType;
  typedef   Vector<int,VectorDimension>                    VDVector;
  typedef   Vector<int,ImageDimension>                     IDVector;

  /** Convenient typedefs. */
  typedef   TInputImage                           InputImageType;
  typedef   TOutputImage                          OutputImageType;
  typedef   Image <unsigned short,ImageDimension> UShortImage;
  typedef   typename TInputImage::IndexType       IndexType;
  typedef   typename TInputImage::SizeType        SizeType;
  typedef   typename TOutputImage::RegionType     RegionType;
  typedef   std::list<IndexType>                  ListType;
  typedef   std::vector<IDVector>                 OffsetType;
  typedef   std::vector<float>                    FloatType;
  
  /** Set/Get the object number be segmented in the input image. */
  itkSetMacro(Objects, int);
  itkGetMacro(Objects, int);

  /** Set/Get the selected object number to be segmented in the input image. */
  itkSetMacro(SelectedObject, int);
  itkGetMacro(SelectedObject, int);  

  /** Setting the covariance matrix for specified object: */
  void SetObjectsMatrix(const MatrixType object_max,const int object_num);

  /** Setting the seed points for specified object. */
  void SetObjectsSeed( const IndexType &seed, const int object_num);

  /** Setting the seed points for specified object. */
  void SetObjectsMean(const VDVector, const int object_num);

  /** Allocate the variate in terms of the number of Objects */
  void Initialization();

protected:
  VectorFuzzyConnectednessImageFilter();
  ~VectorFuzzyConnectednessImageFilter();

  /** Standard pipeline method. */
  void GenerateData();

private:
  SizeType                       m_Size;
  OffsetType                     *m_SpherePointsLoc;
  int                            *m_SpherePointsNum;

  double                         m_Mask[3][3];
  double                         m_MaskTotal;
  VDVector                       m_HomoMaxDiff;
  VDVector                       m_FeaturesThreshold;
  VDVector                       m_PowerValue;
  
  int                            m_Objects;
  int                            m_SelectedObject;

  MatrixType                     *m_ObjectsCovMatrix;
  VDVector                       *m_ObjectsMean;

  VDVector                       *m_ObjectsMaxDiff;
  FloatType                      *m_ObjectsMap;
  ListType                       *m_ObjectsSeed;

  std::vector<float>             m_HomogeneityMap;
  std::vector<float>             m_ScaleMap;
  std::vector<char>              m_ScaleArray;
  std::vector<double>            m_Material;

  typename InputImageType::Pointer   m_InputImage;
  typename InputImageType::Pointer   m_FilterImage;
  typename UShortImage::Pointer      m_ObjectFuzzyScene;
  typename UShortImage::Pointer      m_BackgroundFuzzyScene;
  typename OutputImageType::Pointer  m_SegmentObject; 

  std::vector<unsigned short>  m_Xaffinity;
  std::vector<unsigned short>  m_Yaffinity;
  std::vector<unsigned short>  m_Zaffinity;

  void ScalePrepare();
  void Compute_LookupTable();
  void Compute_Scale();
  void Compute_Filter();
  void Compute_Affinity();
  void FastTracking(int);
};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVectorFuzzyConnectednessImageFilter.txx"
#endif

#endif
