/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkVoronoiSegmentationImageFilterBase.txx
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
#ifndef _itkVoronoiSegmentationImageFilterBase_txx
#define _itkVoronoiSegmentationImageFilterBase_txx

#include "itkImageRegionIteratorWithIndex.h"
#include "itkVoronoiDiagram2DGenerator.h"
#include <math.h>

namespace itk
{

/* constructor: seting the default value of the parameters */
template <class TInputImage, class TOutputImage>
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
VoronoiSegmentationImageFilterBase(){
  m_MinRegion = 20;
  m_Steps = 0;
  m_LastStepSeeds = 0;
  m_NumberOfSeeds = 200;
  m_MeanDeviation = 0.8;
  m_UseBackgroundInAPrior = 0;
  m_OutputBoundary = 0;
}

/* destructor */
template <class TInputImage, class TOutputImage>
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
~VoronoiSegmentationImageFilterBase(){
}

template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Number Of Seeds: " 
     << m_NumberOfSeeds << std::endl;

  os << indent << "Minimum Region for Split: " 
     << m_MinRegion << std::endl;

  os << indent << "Number Of Steps to Run: (0 means runs until no region to split) " 
     << m_Steps << std::endl;
}

/* initialization for the segmentation */
template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
InitializeSegment(void){
  m_InputImage = this->GetInput();
  m_OutputImage = this->GetOutput(); 

  m_Size = m_InputImage->GetLargestPossibleRegion().GetSize();
  IndexType index = IndexType::ZeroIndex;
  RegionType region;
  region.SetSize(m_Size);
  region.SetIndex(index);
  m_OutputImage->SetLargestPossibleRegion( region );
  m_OutputImage->SetBufferedRegion( region );
  m_OutputImage->SetRequestedRegion( region );
  m_OutputImage->Allocate();  

  m_WorkingVD=VoronoiDiagram::New();
  m_VDGenerator=VoronoiDiagramGenerator::New();
 
  VoronoiDiagram::PointType VDsize;
  VDsize[0] = (VoronoiDiagram::CoordRepType)(m_Size[0]-0.1);
  VDsize[1] = (VoronoiDiagram::CoordRepType)(m_Size[1]-0.1);
  m_VDGenerator->SetBoundary(VDsize);
  m_VDGenerator->SetRandomSeeds(m_NumberOfSeeds);
  m_StepsRuned = 0;
}

template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
GetPixelIndexFromPolygon(PointTypeDeque vertlist, IndexList *PixelPool)
{
  IndexType idx;
  PointType currP;
  PointType leftP;
  PointType rightP;
  currP = vertlist.front();
  vertlist.pop_front();
  leftP = vertlist.front();
  while(currP[1] > leftP[1]){
    vertlist.push_back(currP);
    currP=vertlist.front();
    vertlist.pop_front();
    leftP=vertlist.front();
  }
  rightP=vertlist.back();
  while(currP[1] > rightP[1]){
    vertlist.push_front(currP);
    currP=vertlist.back();
    vertlist.pop_back();
    rightP=vertlist.back();
  }
  leftP=vertlist.front();
  PointTypeDeque tmpQ; 
  tmpQ.clear();
  if(leftP[0]>rightP[0]){
    while(!(vertlist.empty())){
      tmpQ.push_back(vertlist.front());
      vertlist.pop_front(); 
    }
    while(!(tmpQ.empty())){
      vertlist.push_front(tmpQ.front());
      tmpQ.pop_front();
    }
  } 
  tmpQ.clear();
  leftP=vertlist.front();
  rightP=vertlist.back();

  double beginy=currP[1];
  int intbeginy=(int)ceil(beginy); 
  idx[1]=intbeginy;
  double leftendy=leftP[1];
  double rightendy=rightP[1];
  double beginx=currP[0];
  double endx=currP[0];
  double leftDx,rightDx;
  double offset;
  double leftheadx=beginx;
  double rightheadx=endx;
  double leftheady=beginy;
  double rightheady=beginy;

  double endy;
  bool RorL;
  int i;
  if(leftendy>rightendy){
    RorL=1;
    endy=rightendy;
  }
  else{
    RorL=0;
    endy=leftendy;
  }
  leftDx=(leftP[0]-beginx)/(leftP[1]-beginy);
  rightDx=(rightP[0]-endx)/(rightP[1]-beginy);
  int intendy=(int)floor(endy);
  if(intbeginy>intendy){ //no scanline
    if(RorL){
      endx=rightP[0];
      beginx+=leftDx*(rightP[1]-beginy);
      beginy=rightP[1];
    }
    else{
      beginx=leftP[0];
      endx+=rightDx*(leftP[1]-beginy); 
      beginy=leftP[1];
    }
  }
  else if((intbeginy==intendy) && (intbeginy==0)){ //only one scanline at 0;
    if(RorL) endx=rightP[0];
    else beginx=leftP[0];
    for(i=ceil(beginx);i<=floor(endx);i++){
      idx[0]=i;
      (*PixelPool).push_back(idx);
    }
    idx[1]=idx[1]+1;
  }
  else{ //normal case some scanlines
    offset=(double)intbeginy-beginy;
    endx+=offset*rightDx;
    beginx+=offset*leftDx;
    while(idx[1]<=intendy){
      for(i=ceil(beginx);i<=floor(endx);i++){
        idx[0]=i;
        (*PixelPool).push_back(idx);
      }
      endx+=rightDx;
      beginx+=leftDx;        
      idx[1]=idx[1]+1;
    }
    beginy=endy;
  }

  int vsize=vertlist.size();
  while(vsize>2){
    vsize--;
    if(RorL){
      vertlist.pop_back();
      currP=rightP;
      rightheadx=currP[0];
      rightheady=currP[1];
      endx=currP[0];
      beginx=leftheadx+leftDx*(beginy-leftheady); 
      rightP=vertlist.back();
      rightDx=(rightP[0]-currP[0])/(rightP[1]-currP[1]);
    }
    else{
      vertlist.pop_front();
      currP=leftP;
      leftheadx=currP[0];
      leftheady=currP[1];
      beginx=currP[0];
      endx=rightheadx+rightDx*(beginy-rightheady);
      leftP=vertlist.front();
      leftDx=(leftP[0]-currP[0])/(leftP[1]-currP[1]);
    }
        
    leftendy=leftP[1];
    rightendy=rightP[1];
    if(leftendy>rightendy){
      RorL=1;
      endy=rightendy;
    }
    else{
      RorL=0;
      endy=leftendy;
    }

    intendy=(int)floor(endy);
    intbeginy=(int)ceil(beginy); 

    if(intbeginy>intendy){ //no scanline
      if(RorL){
        endx=rightP[0];
        beginx+=leftDx*(rightP[1]-beginy);
        beginy=rightP[1];
      }
      else{
        beginx=leftP[0];
        endx+=rightDx*(leftP[1]-beginy); 
        beginy=leftP[1];
      }
    }
    else{ //normal case some scanlines
      offset=(double)intbeginy-beginy;
      endx+=offset*rightDx;
      beginx+=offset*leftDx;
      while(idx[1]<=intendy){
        for(i=ceil(beginx);i<=floor(endx);i++){
          idx[0]=i;
          (*PixelPool).push_back(idx);
        }
        endx+=rightDx;
        beginx+=leftDx;        
        idx[1]=idx[1]+1;
      }
      beginy=idx[1];
    }
  }


  if(RorL){
    beginy=rightP[1];
    endy=leftP[1];
  }
  else{
    beginy=leftP[1];
    endy=rightP[1];
  }
  intbeginy=(int)ceil(beginy);
  intendy=(int)floor(endy);
  if(intbeginy<=intendy){
    if(RorL){
      rightDx=(rightP[0]-leftP[0])/(rightP[1]-leftP[1]);
      endx=rightP[0];
      beginx=leftP[0]+leftDx*(rightP[1]-leftP[1]);
    }
    else{
      leftDx=(rightP[0]-leftP[0])/(rightP[1]-leftP[1]);
      beginx=leftP[0];
      endx=rightP[0]+rightDx*(leftP[1]-rightP[1]);
    }
    offset=(double)intbeginy-beginy;
    beginx+=offset*leftDx;
    endx+=offset*rightDx;
    while(idx[1]<=intendy){
      for(i=ceil(beginx);i<=floor(endx);i++){
        idx[0]=i;
        (*PixelPool).push_back(idx);
      }
      endx+=rightDx;
      beginx+=leftDx;        
      idx[1]=idx[1]+1;
    }
  }
}

template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
ClassifyDiagram(void)
{
  CellPointer currCell; 
  PointIdIterator currPit;
  PointIdIterator currPitEnd;
  PointType currP;
  PointTypeDeque VertList;
  IndexList PixelPool;
  for(int i=0;i<m_NumberOfSeeds;i++){
    currCell = m_WorkingVD->GetCellId(i);
	currPitEnd = currCell->PointIdsEnd();
	VertList.clear();
	for(currPit=currCell->PointIdsBegin();currPit!=currPitEnd;++currPit){
	  m_WorkingVD->GetPoint((*currPit),&(currP));
	  VertList.push_back(currP);
	}

    PixelPool.clear();
	GetPixelIndexFromPolygon(VertList,&PixelPool);
    m_NumberOfPixels[i] = PixelPool.size();
    m_Label[i] = TestHomogeneity(PixelPool);
  }

  m_NumberOfBoundary = 0;
  for(int i=0;i<m_NumberOfSeeds;i++){
    if(m_Label[i] == 0){
	  NeighborIdIterator itend = m_WorkingVD->NeighborIdsEnd(i);
	  NeighborIdIterator it=m_WorkingVD->NeighborIdsBegin(i);
	  bool bnd = 0;
	  while((it != itend) && (!bnd)){
	    bnd = (m_Label[*it] == 1);
		++it;
	  }
	  if(bnd){
	    m_Label[i] = 2;
		m_NumberOfBoundary++;
	  }
	}
  }
}

template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
GenerateAddingSeeds(void)
{
  EdgeIterator eit;
  EdgeIterator eitend =	m_WorkingVD->EdgeEnd();  
  PointType adds;
  Point<int,2> seeds;

  for(eit = m_WorkingVD->EdgeBegin();eit != eitend; ++eit){
	seeds = m_WorkingVD->GetSeedsIDAroundEdge(eit);
	if( ((m_Label[seeds[0]]==2)||(m_Label[seeds[1]]==2))
	  && (m_NumberOfPixels[seeds[0]]>m_MinRegion)
	  && (m_NumberOfPixels[seeds[1]]>m_MinRegion) ){
	  adds[0] = (eit->m_Left[0] + eit->m_Right[0])*0.5;
	  adds[1] = (eit->m_Left[1] + eit->m_Right[1])*0.5;
	  m_SeedsToAdded.push_back(adds);
	}
  }
}


template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
RunSegmentOneStep(void){
  m_NumberOfPixels.resize(m_NumberOfSeeds);
  m_Label.resize(m_NumberOfSeeds);
  m_SeedsToAdded.clear();
  m_VDGenerator->Update();
  m_WorkingVD=m_VDGenerator->GetOutput();
  ClassifyDiagram();
  GenerateAddingSeeds();
  m_NumberOfSeedsToAdded = m_SeedsToAdded.size();
  m_StepsRuned++;
}

template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
RunSegment(void){
  bool ok = 1;
  if(m_Steps == 0){
    RunSegmentOneStep();
	if(m_NumberOfBoundary == 0){
	  ok=0;
    }
    while( (m_NumberOfSeedsToAdded != 0) && ok){
	  m_VDGenerator->AddSeeds(m_NumberOfSeedsToAdded,m_SeedsToAdded.begin());
	  m_LastStepSeeds = m_NumberOfSeeds;
	  m_NumberOfSeeds += m_NumberOfSeedsToAdded;
      RunSegmentOneStep();
	}
  }
  else if(m_Steps == 1){
    RunSegmentOneStep();
  }
  else{
    RunSegmentOneStep();
	if(m_NumberOfBoundary == 0){
	  ok=0;
    }
	int i = 1;
	while((i<m_Steps) && ok){
	  m_VDGenerator->AddSeeds(m_NumberOfSeedsToAdded, m_SeedsToAdded.begin());
	  m_LastStepSeeds = m_NumberOfSeeds;
	  m_NumberOfSeeds += m_NumberOfSeedsToAdded;
      RunSegmentOneStep();
	  i++;
	}
  }	  
}

template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
GenerateData(void){ 
  RunSegment();
  if(m_OutputBoundary)
    MakeSegmentBoundary();
  else
    MakeSegmentObject();
}

template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
BeforeNextStep(void){ 
  m_VDGenerator->AddSeeds(m_NumberOfSeedsToAdded, m_SeedsToAdded.begin()); 
  m_LastStepSeeds = m_NumberOfSeeds; 
  m_NumberOfSeeds += m_NumberOfSeedsToAdded; 
} 


template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
MakeSegmentBoundary(void)
{

  RegionType region = m_InputImage->GetRequestedRegion(); 
  itk::ImageRegionIteratorWithIndex <OutputImageType> oit(m_OutputImage, region); 
  while( !oit.IsAtEnd())
    {     
    oit.Set(0); 
    ++oit; 
    }

  NeighborIdIterator nit;
  NeighborIdIterator nitend;
  for(int i=0;i<m_NumberOfSeeds;i++){
    if(m_Label[i] == 2){
      nitend = m_WorkingVD->NeighborIdsEnd(i);
	    for(nit=m_WorkingVD->NeighborIdsBegin(i);nit!=nitend;++nit){
	      if(((*nit)>i)&&(m_Label[*nit]==2)){
		      drawLine(m_WorkingVD->GetSeed(i),m_WorkingVD->GetSeed(*nit));
		      i=i;
	      }
	    }
    }
  }
}

template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
MakeSegmentObject(void)
{
  RegionType region = m_InputImage->GetRequestedRegion(); 
  itk::ImageRegionIteratorWithIndex <OutputImageType> oit(m_OutputImage, region); 
  while( !oit.IsAtEnd())
    {     
    oit.Set(0); 
    ++oit; 
    }
  CellPointer currCell; 
  PointIdIterator currPit;
  PointIdIterator currPitEnd;
  PointType currP;
  PointTypeDeque VertList;
  for(int i=0;i<m_NumberOfSeeds;i++){
    if(m_Label[i] == 1){
      currCell = m_WorkingVD->GetCellId(i);
      currPitEnd = currCell->PointIdsEnd();
	  VertList.clear();
	  for(currPit=currCell->PointIdsBegin();currPit!=currPitEnd;++currPit){
	    m_WorkingVD->GetPoint((*currPit),&(currP));
	    VertList.push_back(currP);
	  }
	  FillPolygon(VertList);
    }
  }
}

template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
FillPolygon(PointTypeDeque vertlist)
{
  IndexType idx;

  PointType currP;
  PointType leftP;
  PointType rightP;
  currP = vertlist.front();
  vertlist.pop_front();
  leftP = vertlist.front();
  while(currP[1] > leftP[1]){
    vertlist.push_back(currP);
    currP=vertlist.front();
    vertlist.pop_front();
    leftP=vertlist.front();
  }
  rightP=vertlist.back();
  while(currP[1] > rightP[1]){
    vertlist.push_front(currP);
    currP=vertlist.back();
    vertlist.pop_back();
    rightP=vertlist.back();
  }
  leftP=vertlist.front();
  PointTypeDeque tmpQ; 
  tmpQ.clear();
  if(leftP[0]>rightP[0]){
    while(!(vertlist.empty())){
      tmpQ.push_back(vertlist.front());
      vertlist.pop_front(); 
    }
    while(!(tmpQ.empty())){
      vertlist.push_front(tmpQ.front());
      tmpQ.pop_front();
    }
  } 
  tmpQ.clear();
  leftP=vertlist.front();
  rightP=vertlist.back();

  double beginy=currP[1];
  int intbeginy=(int)ceil(beginy); 
  idx[1]=intbeginy;
  double leftendy=leftP[1];
  double rightendy=rightP[1];
  double beginx=currP[0];
  double endx=currP[0];
  double leftDx,rightDx;
  double offset;
  double leftheadx=beginx;
  double rightheadx=endx;
  double leftheady=beginy;
  double rightheady=beginy;

  double endy;
  bool RorL;
  int i;
  if(leftendy>rightendy){
    RorL=1;
    endy=rightendy;
  }
  else{
    RorL=0;
    endy=leftendy;
  }
  leftDx=(leftP[0]-beginx)/(leftP[1]-beginy);
  rightDx=(rightP[0]-endx)/(rightP[1]-beginy);
  int intendy=(int)floor(endy);
  if(intbeginy>intendy){ //no scanline
    if(RorL){
      endx=rightP[0];
      beginx+=leftDx*(rightP[1]-beginy);
      beginy=rightP[1];
    }
    else{
      beginx=leftP[0];
      endx+=rightDx*(leftP[1]-beginy); 
      beginy=leftP[1];
    }
  }
  else if((intbeginy==intendy) && (intbeginy==0)){ //only one scanline at 0;
    if(RorL) endx=rightP[0];
    else beginx=leftP[0];
    for(i=ceil(beginx);i<=floor(endx);i++){
      idx[0]=i;
      m_OutputImage->SetPixel(idx,1);  
    }
    idx[1]=idx[1]+1;
  }
  else{ //normal case some scanlines
    offset=(double)intbeginy-beginy;
    endx+=offset*rightDx;
    beginx+=offset*leftDx;
    while(idx[1]<=intendy){
      for(i=ceil(beginx);i<=floor(endx);i++){
        idx[0]=i;
        m_OutputImage->SetPixel(idx,1);  
      }
      endx+=rightDx;
      beginx+=leftDx;        
      idx[1]=idx[1]+1;
    }
    beginy=endy;
  }

  int vsize=vertlist.size();
  while(vsize>2){
    vsize--;
    if(RorL){
      vertlist.pop_back();
      currP=rightP;
      rightheadx=currP[0];
      rightheady=currP[1];
      endx=currP[0];
      beginx=leftheadx+leftDx*(beginy-leftheady); 
      rightP=vertlist.back();
      rightDx=(rightP[0]-currP[0])/(rightP[1]-currP[1]);
    }
    else{
      vertlist.pop_front();
      currP=leftP;
      leftheadx=currP[0];
      leftheady=currP[1];
      beginx=currP[0];
      endx=rightheadx+rightDx*(beginy-rightheady);
      leftP=vertlist.front();
      leftDx=(leftP[0]-currP[0])/(leftP[1]-currP[1]);
    }
        
    leftendy=leftP[1];
    rightendy=rightP[1];
    if(leftendy>rightendy){
      RorL=1;
      endy=rightendy;
    }
    else{
      RorL=0;
      endy=leftendy;
    }

    intendy=(int)floor(endy);
    intbeginy=(int)ceil(beginy); 

    if(intbeginy>intendy){ //no scanline
      if(RorL){
        endx=rightP[0];
        beginx+=leftDx*(rightP[1]-beginy);
        beginy=rightP[1];
      }
      else{
        beginx=leftP[0];
        endx+=rightDx*(leftP[1]-beginy); 
        beginy=leftP[1];
      }
    }
    else{ //normal case some scanlines
      offset=(double)intbeginy-beginy;
      endx+=offset*rightDx;
      beginx+=offset*leftDx;
      while(idx[1]<=intendy){
        for(i=ceil(beginx);i<=floor(endx);i++){
          idx[0]=i;
          m_OutputImage->SetPixel(idx,1);  
        }
        endx+=rightDx;
        beginx+=leftDx;        
        idx[1]=idx[1]+1;
      }
      beginy=idx[1];
    }
  }


  if(RorL){
    beginy=rightP[1];
    endy=leftP[1];
  }
  else{
    beginy=leftP[1];
    endy=rightP[1];
  }
  intbeginy=(int)ceil(beginy);
  intendy=(int)floor(endy);
  if(intbeginy<=intendy){
    if(RorL){
      rightDx=(rightP[0]-leftP[0])/(rightP[1]-leftP[1]);
      endx=rightP[0];
      beginx=leftP[0]+leftDx*(rightP[1]-leftP[1]);
    }
    else{
      leftDx=(rightP[0]-leftP[0])/(rightP[1]-leftP[1]);
      beginx=leftP[0];
      endx=rightP[0]+rightDx*(leftP[1]-rightP[1]);
    }
    offset=(double)intbeginy-beginy;
    beginx+=offset*leftDx;
    endx+=offset*rightDx;
    while(idx[1]<=intendy){
      for(i=ceil(beginx);i<=floor(endx);i++){
        idx[0]=i;
        m_OutputImage->SetPixel(idx,1);  
      }
      endx+=rightDx;
      beginx+=leftDx;        
      idx[1]=idx[1]+1;
    }
  }

}

template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>::
drawLine(PointType p1,PointType p2){
  int x1=(int)(p1[0]+0.5);
  int x2=(int)(p2[0]+0.5);
  int y1=(int)(p1[1]+0.5);
  int y2=(int)(p2[1]+0.5);
  if(x1==m_Size[0]) x1--;  
  if(x2==m_Size[0]) x2--;  
  if(y1==m_Size[1]) y1--;  
  if(y2==m_Size[1]) y2--;  


  int dx=x1-x2;
  int adx=(dx>0)?dx:-dx;
  int dy=y1-y2;
  int ady=(dy>0)?dy:-dy;
  int save;
  float curr;
  IndexType idx;
  if (adx > ady){
    if(x1>x2){
	  save=x1; x1=x2; x2=save;
	  save=y1; y1=y2; y2=save;
	}
    curr=(float)y1;
    float offset=(float)dy/dx;
	for(int i=x1;i<=x2;i++){
	  idx[0]=i;
	  idx[1]=y1;
	  m_OutputImage->SetPixel(idx,1);
	  curr += offset;
  	  y1=(int)(curr+0.5);
    }
  }
  else { 
    if(y1>y2){
	  save=x1; x1=x2; x2=save;
	  save=y1; y1=y2; y2=save;
	}
    curr=(float)x1;
    float offset=(float)dx/dy;
	for(int i=y1;i<=y2;i++){
	  idx[0]=x1;
	  idx[1]=i;
	  m_OutputImage->SetPixel(idx,1);
	  curr += offset;
	  x1=(int)(curr+0.5);
    }
  }

}

template <class TInputImage, class TOutputImage> 
void 
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>:: 
DrawDiagram(VDImagePointer result,unsigned char incolor, 
    unsigned char outcolor,unsigned char boundcolor) 
{ 
  
  RegionType region = m_InputImage->GetRequestedRegion(); 
  itk::ImageRegionIteratorWithIndex <VDImage> vdit(result, region); 
  while( !vdit.IsAtEnd())
    {     
    vdit.Set(0); 
    ++vdit; 
    } 
      
  EdgeIterator eit; 
  EdgeIterator eitend = m_WorkingVD->EdgeEnd();   
  PointType adds; 
  Point<int,2> seeds; 
  for(eit = m_WorkingVD->EdgeBegin();eit != eitend; ++eit){ 
    seeds = m_WorkingVD->GetSeedsIDAroundEdge(eit); 
    if((m_Label[seeds[0]]==2)||(m_Label[seeds[1]]==2)){ 
      drawVDline(result,eit->m_left,eit->m_right,boundcolor); 
    } 
    else if(m_Label[seeds[0]]){ 
      drawVDline(result,eit->m_left,eit->m_right,incolor); 
    } 
    else { 
      drawVDline(result,eit->m_left,eit->m_right,outcolor); 
    } 
  } 
    
} 
    
template <class TInputImage, class TOutputImage> 
void 
VoronoiSegmentationImageFilterBase <TInputImage,TOutputImage>:: 
drawVDline(VDImagePointer result,PointType p1,PointType p2, unsigned char color) 
{ 
  int x1=(int)(p1[0]+0.5); 
  int x2=(int)(p2[0]+0.5); 
  int y1=(int)(p1[1]+0.5); 
  int y2=(int)(p2[1]+0.5); 
  if(x1==m_Size[0]) x1--;  
  if(x2==m_Size[0]) x2--;  
  if(y1==m_Size[1]) y1--;  
  if(y2==m_Size[1]) y2--; 
  int dx=x1-x2; 
  int adx=(dx>0)?dx:-dx; 
  int dy=y1-y2; 
  int ady=(dy>0)?dy:-dy; 
  int save; 
  float curr; 
  IndexType idx; 
  if (adx > ady){ 
    if(x1>x2){ 
      save=x1; x1=x2; x2=save; 
      save=y1; y1=y2; y2=save; 
    } 
    curr=(float)y1; 
    float offset=(float)dy/dx; 
    for(int i=x1;i<=x2;i++){ 
      idx[0]=i; 
      idx[1]=y1; 
      result->SetPixel(idx,color); 
      curr += offset; 
      y1=(int)(curr+0.5); 
    } 
  } 
  else {  
    if(y1>y2){ 
      save=x1; x1=x2; x2=save; 
      save=y1; y1=y2; y2=save; 
    } 
    curr=(float)x1; 
    float offset=(float)dx/dy; 
    for(int i=y1;i<=y2;i++){ 
      idx[0]=x1; 
      idx[1]=i; 
      result->SetPixel(idx,color); 
      curr += offset; 
      x1=(int)(curr+0.5); 
    } 
  } 
} 

} //end namespace

#endif


