/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkVoronoiSegmentationImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkVoronoiSegmentationImageFilter_txx
#define _itkVoronoiSegmentationImageFilter_txx

#include "itkImageRegionIteratorWithIndex.h"


namespace itk
{

/* constructor: seting the default value of the parameters */
template <class TInputImage, class TOutputImage>
VoronoiSegmentationImageFilter <TInputImage,TOutputImage>
::VoronoiSegmentationImageFilter()
{
  m_MeanPercentError = 0.10;
  m_VarPercentError = 1.5;
  m_Mean = m_Var = m_MeanTolerance = m_VarTolerance = 0.0;
}

/* destructor */
template <class TInputImage, class TOutputImage>
VoronoiSegmentationImageFilter <TInputImage,TOutputImage>
::~VoronoiSegmentationImageFilter()
{
}

template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilter <TInputImage,TOutputImage>
::SetMeanPercentError(double x)
{
  m_MeanPercentError = x;
  m_MeanTolerance = x*m_Mean;
}


template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilter <TInputImage,TOutputImage>
::SetVarPercentError(double x)
{
  m_VarPercentError = x;
  m_VarTolerance = x*m_Var;
}

template <class TInputImage, class TOutputImage>
bool
VoronoiSegmentationImageFilter <TInputImage,TOutputImage>
::TestHomogeneity(IndexList Plist)
{
  int num=Plist.size();
  int i;
  double getp;
  double addp=0;
  double addpp=0;
  for(i=0;i<num;i++)
    {
    getp = (double)(this->GetInput()->GetPixel(Plist[i]));
    addp=addp+getp;
    addpp=addpp+getp*getp;
    }

  double savemean,savevar;
  if(num > 1)
    {
    savemean = addp/num;
    savevar = sqrt((addpp - (addp*addp)/(num) )/(num-1));
    }
  else
    {
    savemean = 0;
    savevar = -1;
    }

  savemean -= m_Mean;
  savevar -= m_Var;
  if( (savemean>-m_MeanTolerance) && (savemean<m_MeanTolerance) 
      && (savevar<m_VarTolerance) )
    {
    return 1;
    }
  else
    {
    return 0;
    }
}




template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilter <TInputImage,TOutputImage>
::TakeAPrior(BinaryObjectImage* aprior)
{
  RegionType region = this->GetInput()->GetRequestedRegion();
  itk::ImageRegionIteratorWithIndex <BinaryObjectImage> ait(aprior, region);
  itk::ImageRegionIteratorWithIndex <InputImageType> iit(this->GetInput(), region);

  int num=0;
  float addp=0;
  float addpp=0;
  float currp;

  unsigned int i,j;
  unsigned int minx,miny,maxx,maxy;
  bool status=0;
  for(i=0;i<m_Size[1];i++)
    {
    for(j=0;j<m_Size[0];j++)
      {
      if( (status==0)&&(ait.Get()) )
        {
        miny=i;
        minx=j;
        maxy=i;
        maxx=j;
        status=1;
        } 
      else if( (status==1)&&(ait.Get()) )
        {
        maxy=i;
        if(minx>j) minx=j;
        if(maxx<j) maxx=j;
        }  
      ++ait;
      }
    }       
  
  float addb=0;
  float addbb=0;
  int numb=0;

  ait.GoToBegin();
  iit.GoToBegin();
  for(i=0;i<miny;i++)
    {
    for(j=0;j<m_Size[0];j++)
      {
      ++ait;
      ++iit;
      }
    }
  for(i=miny;i<=maxy;i++)
    {
    for(j=0;j<minx;j++)
      {
      ++ait;
      ++iit;
      }
    for(j=minx;j<=maxx;j++)
      {
      if(ait.Get())
        {
        num++;
        currp = (float)(iit.Get());
        addp += currp;
        addpp += currp*currp;
        }
      else
        {
        numb++;
        currp = (float)(iit.Get());
        addb += currp;
        addbb += currp*currp;
        }
      ++ait;++iit;
      }
    for(j=maxx+1;j<m_Size[0];j++)
      {
      ++ait;
      ++iit;
      }
  }

  m_Mean = addp/num;
  m_Var = sqrt((addpp - (addp*addp)/num)/(num-1));
  float b_Mean = addb/numb;

  if(m_UseBackgroundInAPrior)
    {
    m_MeanTolerance = fabs(m_Mean-b_Mean)*m_MeanDeviation;
    }
  else
    {
    m_MeanTolerance = m_Mean*m_MeanPercentError;
    }
  m_VarTolerance = m_Var*m_VarPercentError;
}


template <class TInputImage, class TOutputImage>
void
VoronoiSegmentationImageFilter <TInputImage,TOutputImage>
::Reset(void)
{
  m_VDGenerator->SetRandomSeeds(m_NumberOfSeeds);
  m_StepsRuned = 0;
  m_LastStepSeeds=m_NumberOfSeeds;
  m_NumberOfSeedsToAdded=0;
}

} //end namespace

#endif

