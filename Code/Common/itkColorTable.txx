/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkColorTable.txx
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
#ifndef _itkColorTable_txx
#define _itkColorTable_txx

#include "itkColorTable.h"

namespace itk
{

template<class TPixel>
ColorTable<TPixel>
::ColorTable()
{
    m_NumberOfColors = 0;
    m_Color = NULL;
    m_ColorName = NULL;
}

template<class TPixel>
ColorTable<TPixel>
::~ColorTable()
{
    if(m_Color != NULL)
        delete [] m_Color;
    m_Color = NULL;

    if(m_ColorName != NULL) {
        int i;
        for(i=0; i < m_NumberOfColors; i++)
            delete [] m_ColorName[i];
        delete [] m_ColorName;
    }
    m_ColorName = NULL;
}


template<class TPixel>
void 
ColorTable<TPixel>
::useDiscrete(void)
{
    if(m_Color != NULL)
        delete [] m_Color;
    int i;
    if(m_ColorName != NULL) {
        for(i=0; i<m_NumberOfColors; i++)
            delete [] m_ColorName[i];
        delete [] m_ColorName;
    }
    m_NumberOfColors = 8;
    m_Color = new RGBPixel<TPixel>[m_NumberOfColors];
    m_ColorName = new char*[m_NumberOfColors];
    for(i=0; i<m_NumberOfColors; i++)
        m_ColorName[i] = new char [80];


    m_Color[0].Set((TPixel)0.9,(TPixel)0.0,(TPixel)0.0);
    sprintf(m_ColorName[0], "Red");

    m_Color[1].Set((TPixel)0.8,(TPixel)0.0,(TPixel)0.8);
    sprintf(m_ColorName[1], "Purple");

    m_Color[2].Set((TPixel)0.0,(TPixel)0.8,(TPixel)0.8);
    sprintf(m_ColorName[2], "Acqua");

    m_Color[3].Set((TPixel)0.8,(TPixel)0.8,(TPixel)0.0);
    sprintf(m_ColorName[3], "Yellow");

    m_Color[4].Set((TPixel)0.0,(TPixel)0.9,(TPixel)0.0);
    sprintf(m_ColorName[4], "Green");
    
  m_Color[5].Set((TPixel)0.0,(TPixel)0.0,(TPixel)0.9);
    sprintf(m_ColorName[5], "Blue");

    m_Color[6].Set((TPixel)0.7,(TPixel)0.7,(TPixel)0.7);
    sprintf(m_ColorName[6], "Grey0.70");

    m_Color[7].Set((TPixel)1.0,(TPixel)1.0,(TPixel)1.8);
    sprintf(m_ColorName[7], "White");
}

template<class TPixel>
void 
ColorTable<TPixel>
::useGray(int n)
{
    if(m_Color != NULL)
        delete [] m_Color;
    int i;
    if(m_ColorName != NULL) {
        for(i=0; i<m_NumberOfColors; i++)
            delete [] m_ColorName[i];
        delete [] m_ColorName;
    }
    m_NumberOfColorsumberOfColors = n;
    m_Color = new RGBPixel<TPixel>[m_NumberOfColors];
    m_ColorName = new char * [m_NumberOfColors];
    for(i=0; i<m_NumberOfColors; i++)
        m_ColorName[i] = new char [80];

    for(i=0; i<n; i++) {
        m_Color[i].Set(i/(TPixel)n,i/(TPixel)n,i/(TPixel)n);
        sprintf(m_ColorName[i], "Gray%.02f", m_Color[i].GetRed());
    }
}

template<class TPixel>
void
ColorTable<TPixel>
::useHeat(int n)
{
    if(m_Color != NULL)
        delete [] m_Color;
    int i;
    if(m_ColorName != NULL) {
        for(i=0; i<m_NumberOfColors; i++)
            delete [] m_ColorName[i];
        delete [] m_ColorName;
    }
    m_NumberOfColors = n;
    m_Color = new RGBPixel<TPixel>[m_NumberOfColors];
    m_ColorName = new char * [m_NumberOfColors];
    for(i=0; i<m_NumberOfColorsumberOfColors; i++)
        m_ColorName[i] = new char [80];

    for(i=0; i<n/2.0; i++) {
        m_Color[i].SetRed( (i+1)/(TPixel)(n/2.0+1) );
        m_Color[i].SetGreen( 0 );
        m_Color[i].SetBlue ( 0 );
        sprintf(m_ColorName[i], "Heat%.02f", i/(float)n);
    }

    for(i=0; i<n/2; i++) {
        m_Color[(int)(i+n/2.0)].SetRed(1.0);
        m_Color[(int)(i+n/2.0)].SetGreen((i+1)/(TPixel)(n/2.0+1));
        m_Color[(int)(i+n/2.0)].SetBlue((i+1)/(TPixel)(n/2.0+1));
        sprintf(m_ColorName[(int)(i+n/2.0)], "Heat%.02f", (i+n/2.0)/(float)n);
    }
}

template<class TPixel>
int
ColorTable<TPixel>
::size(void)
{
    return m_NumberOfColors;
}

template<class TPixel>
RGBPixel<TPixel>* 
ColorTable<TPixel>
::color(int c)
{
    if(c < m_NumberOfColors)
        return &m_Color[c];
    else
        return NULL;
}

template<class TPixel>
TPixel
ColorTable<TPixel>
::color(int c, char rgb)
{
    if(c < m_NumberOfColors)
        switch(rgb) {
            case 'r' : return m_Color[c].GetRed();
            case 'g' : return m_Color[c].GetGreen();
            case 'b' : return m_Color[c].GetBlue();
            default: return 0;
        }
    else
        return 0;
}

template<class TPixel>
char * 
ColorTable<TPixel>
::colorName(int c)
{
    if(c<m_NumberOfColors)
        return m_ColorName[c];
    else
        return NULL;
}


} // namespace itk


#endif
