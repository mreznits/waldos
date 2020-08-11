/*************************************************************************
	"Where's Senor Waldos?"
	Miovision Programming Contest
 
	Copyright Maxim Reznitskii
	Version 1: Dec. 21, 2007
	Version 2: Aug. 10, 2020

	Created with OpenCV 1.0 and Microsoft Visual Studio 2008

	Input.h = Class for processing an input image
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

#ifndef _INPUT_H
#define _INPUT_H

#include <string>

#include "cv.h"
#include "highgui.h" 
#include <cxcore.h>

class Input
{
	IplImage * m_imgBGR;
	IplImage * m_imgRed;
	IplImage * m_imgWhite;

	CvSize m_size;

	//-----------------------------------------------------------------------------------------------------
	// Convert the image to HSV
	// Threshold it to keep only red and white
	//-----------------------------------------------------------------------------------------------------
	void filterColours()
	{
		IplImage * imgHsv = cvCreateImage( this->m_size, IPL_DEPTH_8U, 3 );
		IplImage * imgHue = cvCreateImage( this->m_size, IPL_DEPTH_8U, 1 );
		IplImage * imgSat = cvCreateImage( this->m_size, IPL_DEPTH_8U, 1 );
		IplImage * imgVal = cvCreateImage( this->m_size, IPL_DEPTH_8U, 1 );

		//convert bgr image to hsv color scheme
		cvCvtColor( this->m_imgBGR, imgHsv, CV_BGR2HSV );
		cvSplit(imgHsv, imgHue, imgSat, imgVal, NULL);

		int w = imgHsv->width;
		int h = imgHsv->height;

		cvZero(this->m_imgRed);
		cvZero(this->m_imgWhite);

		// TODO: Clever thresholding of the image would be more efficient
		// than this looping over pixels
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++)
			{
				//get the (x, y) pixel value 
				double h = cvGet2D(imgHue, y, x).val[0];
				double s = cvGet2D(imgSat, y, x).val[0];
				double v = cvGet2D(imgVal, y, x).val[0];		

				//in OpenCV, hue, saturation, and value are all out of 255
				//in standard HSV, hue is out of 360 and saturation and value are out of 100
				if ( (h > 350*255/360 || h < 10*255/360) && s > 90*255/100 )
				{
					// red hue and high saturation -> pixel is a shade of red 
					cvSet2D( this->m_imgRed, y, x, cvScalar(1) );
				}
				else if ( s <= 31*255/100 )
				{
					// low saturation -> pixel is a shade of white
					cvSet2D( this->m_imgWhite, y, x, cvScalar(1) );
				}
				else if ( (h > 240*255/360 || h < 30*255/360) && v > 30*255/100 )
				{
					// brown or purple hue and value at least 30/100 -> pixel is a shade of red
					//(must accept these b/c they appear in some gradients between red & white)
					cvSet2D( this->m_imgRed, y, x, cvScalar(1) );
				}
			}

		cvReleaseImage( &imgHsv );
		cvReleaseImage( &imgHue );
		cvReleaseImage( &imgSat );
		cvReleaseImage( &imgVal );
	}

public:
	Input(std::string _filePath, bool _bShowRedWhite)
	{
		m_imgBGR = cvLoadImage(_filePath.c_str());
		m_size = cvSize(m_imgBGR->width, m_imgBGR->height);

		m_imgRed = cvCreateImage( this->m_size, IPL_DEPTH_8U, 1 );
		m_imgWhite = cvCreateImage( this->m_size, IPL_DEPTH_8U, 1 );

		//Idea: Waldo's shirt is always red and white -> apply colour-based filters
		filterColours();

		if (_bShowRedWhite)
		{
			cvNamedWindow("red");
			cvNamedWindow("white");
			this->showRed("red");
			this->showWhite("white");
		}
	}

	~Input()
	{
		cvReleaseImage( &m_imgBGR );
		cvReleaseImage( &m_imgRed );
		cvReleaseImage( &m_imgWhite );

		cvDestroyWindow("red");
		cvDestroyWindow("white");
	}

	IplImage * getImgBgr()
	{
		return m_imgBGR;
	}

	IplImage * getImgRed()
	{
		return m_imgRed;
	}

	IplImage * getImgWhite()
	{
		return m_imgWhite;
	}

	CvSize getSize()
	{
		return m_size;
	}

	void setROI(CvRect _rect)
	{
		cvSetImageROI(this->m_imgRed, _rect);
		cvSetImageROI(this->m_imgWhite, _rect);
	}

	void resetROI()
	{
		cvResetImageROI(this->m_imgRed);
		cvResetImageROI(this->m_imgWhite);
	}

	void showRed(std::string _winName)
	{
		IplImage * imgTemp = cvCreateImage( this->m_size, IPL_DEPTH_8U, 1 );
		
		cvCvtScale(this->m_imgRed, imgTemp, 255);
		cvShowImage(_winName.c_str(), imgTemp);

		cvReleaseImage(&imgTemp);
	}

	void showWhite(std::string _winName)
	{
		IplImage * imgTemp = cvCreateImage( this->m_size, IPL_DEPTH_8U, 1 );
		
		cvCvtScale(this->m_imgWhite, imgTemp, 255);
		cvShowImage(_winName.c_str(), imgTemp);

		cvReleaseImage(&imgTemp);
	}

	void showBgrWithRect(std::string _winName, CvPoint _pt1, CvPoint _pt2)
	{
		IplImage * temp = cvCloneImage(this->m_imgBGR);
		cvRectangle(temp, _pt1, _pt2, CV_RGB(255, 255, 255), 3);
		cvShowImage(_winName.c_str(), temp);
		cvReleaseImage(&temp);
	}

	void showBgr(std::string _winName)
	{
		cvShowImage(_winName.c_str(), this->m_imgBGR);
	}
};

#endif
