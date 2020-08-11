/*************************************************************************
	"Where's Senor Waldos?"
	Miovision Programming Contest
 
	Copyright Maxim Reznitskii
	Version 1: Dec. 21, 2007
	Version 2: Aug. 10, 2020

	Created with OpenCV 1.0 and Microsoft Visual Studio 2008

	Mask.h = Class for creating a mask of fixed dimensions
*************************************************************************/
/*************************************************************************
    This file is part of Waldos.

    Waldos is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Waldos is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Waldos.  If not, see <https://www.gnu.org/licenses/>.
*************************************************************************/

#ifndef _MASK_H
#define _MASK_H

#include "cv.h"

class Mask
{
	int m_maskW;
	int m_maskH;
	int m_stripeH;

	IplImage * m_imgMask;
	IplImage * m_imgMaskInv;

	void GenerateMask()
	{
		cvZero(m_imgMask);

		//Generate a mask with alternating black and white stripes
		//Ex for 9x9: 2B + 2W + 2B + 2W + 1B
		cvSetImageROI(m_imgMask, cvRect(0, 0, this->m_maskW, this->m_stripeH));
		cvSet(m_imgMask, cvScalar(0)); //black
		cvSetImageROI(m_imgMask, cvRect(0, this->m_stripeH, this->m_maskW, this->m_stripeH));
		cvSet(m_imgMask, cvScalar(1)); //white
		cvSetImageROI(m_imgMask, cvRect(0, 2*this->m_stripeH, this->m_maskW, this->m_stripeH));
		cvSet(m_imgMask, cvScalar(0)); //black
		cvSetImageROI(m_imgMask, cvRect(0, 3*this->m_stripeH, this->m_maskW, this->m_stripeH));
		cvSet(m_imgMask, cvScalar(1)); //white
		cvSetImageROI(m_imgMask, cvRect(0, 3*this->m_stripeH + this->m_stripeH, this->m_maskW, this->m_stripeH));
		cvSet(m_imgMask, cvScalar(0)); //black

		cvResetImageROI(m_imgMask);

		cvSet(m_imgMaskInv, cvScalar(1));
		cvSub(m_imgMaskInv, m_imgMask, m_imgMaskInv);
	}

public:
	Mask(int _maskW, int _maskH)
	{
		m_imgMask = cvCreateImage( cvSize(_maskW, _maskH), IPL_DEPTH_8U, 1 );
		m_imgMaskInv = cvCreateImage( cvSize(_maskW, _maskH), IPL_DEPTH_8U, 1 );

		m_maskW = _maskW;
		m_maskH = _maskH;
		m_stripeH = (_maskH - 1) / 4;

		GenerateMask();
	}

	~Mask()
	{
		cvReleaseImage( &m_imgMask );
		cvReleaseImage( &m_imgMaskInv );
	}

	int getH()
	{
		return m_maskH;
	}

	int getW()
	{
		return m_maskW;
	}

	IplImage * getImg()
	{
		return m_imgMask;
	}

	IplImage * getImgInv()
	{
		return m_imgMaskInv;
	}
};

#endif