/*************************************************************************
	"Where's Senor Waldos?"
	Miovision Programming Contest
 
	Copyright Maxim Reznitskii
	Version 1: Dec. 21, 2007
	Version 2: Aug. 10, 2020

	Created with OpenCV 1.0 and Microsoft Visual Studio 2008

	Waldos.cpp - Core algorithm functions for finding Waldos
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

#include "Waldos.h"

#include <ctime>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <numeric>

using namespace std;

CvPoint findWaldo(Input * _input, bool _bDebug)
{
	IplImage * imgMatch = cvCreateImage( _input->getSize(), IPL_DEPTH_8U, 1 );

	// Idea: Waldo's shirt is always striped -> search for it using a mask of stripes
	// Idea: In the images, Waldo is always vertical -> shirt stripes are always horizontal
	findMaskMatchLoc( _input, _bDebug, *imgMatch );
	CvPoint center = getCenterOfLargestBlob( imgMatch );

	cvReleaseImage( &imgMatch );

	return center;
}

void findMaskMatchLoc(Input * _input, bool _bDebug, IplImage & _imgDst)
{
	int inputW = _input->getSize().width;
	int inputH = _input->getSize().height;

	Mask * mask;

	//image for storing intermediate results
	IplImage * imgTemp = cvCreateImage( _input->getSize(), IPL_DEPTH_8U, 1 );

	double quality;
	double bestQuality = 0.0;
	int bestMaskH = 0;

	int minMaskSize, maxMaskSize, maskStepSize;
	getOptimalMaskParams(inputW, inputH, minMaskSize, maxMaskSize, maskStepSize);

#ifdef _DEBUG_ALL_MASKS
	cvNamedWindow("mask full out", 0);
#ifdef _DEBUG_Y
	cvNamedWindow("mask_", 0);
	cvNamedWindow("mask small out", 0);
#endif
	printf("From %dx%d to %dx%d, step size = %d\n", minMaskSize, minMaskSize, 
	maxMaskSize, maxMaskSize, maskStepSize);
#endif

	for (int maskH = minMaskSize; maskH <= maxMaskSize; maskH += maskStepSize)
	{
		// Idea: since the mask is invariant along x, can make it the entire width of the image
		mask = new Mask(inputW, maskH);
		time_t before = time(0); 

#ifdef _DEBUG_ALL_MASKS
		printf("Attempting %d x %d mask\n", maskH, maskH);
#ifdef _DEBUG_Y
		_input->showBgrWithRect("src", cvPoint(0, _DEBUG_Y - (mask->getH() - 1) / 2),
			cvPoint(mask->getW(), _DEBUG_Y + (mask->getH() - 1) / 2));
#endif
#endif

		applyMaskToFullImg(_input, mask, *imgTemp, quality);

		time_t after = time(0); 
		double duration = difftime(after, before);

#ifdef _DEBUG_ALL_MASKS
		printf("Done (%f sec). ", duration);
#endif

		if (quality < 0.6)
		{
			// reject results where the best ratio (num matched pixels / total num pixels) for any
			// location was below 0.6 (a black square will match a mask with a ratio of 0.55)
#ifdef _DEBUG_ALL_MASKS
			printf("Best ratio (X,Y) below threshold.\n");
#endif
			cvZero(imgTemp);
		}
		else if (quality > bestQuality)
		{
			// this is the best result we've seen so far
			cvCopy(imgTemp, &_imgDst);
			bestQuality = quality;
			bestMaskH = maskH;
#ifdef _DEBUG_ALL_MASKS
			printf("Best ratio (X,Y) = %.2f. Mask selected.\n", quality);
#endif
		}
		else
		{
#ifdef _DEBUG_ALL_MASKS
			printf("Best ratio (X,Y) = %.2f. Mask not selected.\n", quality);
#endif
		}

#ifdef _DEBUG_ALL_MASKS
		showBinaryImage("mask full out", imgTemp);
		cvWaitKey(0);
#endif

		delete mask;
	}

#ifdef _DEBUG_ALL_MASKS
	cvDestroyWindow("mask full out");
#ifdef _DEBUG_Y
	cvDestroyWindow("mask_");
	cvDestroyWindow("mask small out");
#endif
#endif

	if (_bDebug)
	{
		printf("Best mask = %d x %d\n", bestMaskH, bestMaskH);
		showBinaryImage("result of best mask", &_imgDst);
	}

	cvReleaseImage( &imgTemp );
}

void getOptimalMaskParams(int _width, int _height, int & _minMaskSize, int & _maxMaskSize, int & _maskStepSize)
{
	int numMasks = 6;

	_minMaskSize = (int)floor((double)min(_width, _height) / 48.0);
	if (_minMaskSize % 2 == 0) _minMaskSize -= 1; //minMaskSize has to be an odd number
	_minMaskSize = max(9, _minMaskSize); // smallest acceptable mask size is 9

	// initial estimate of max mask size
	_maxMaskSize = (int)floor((double)min(_width, _height) / 12.0); 

	_maskStepSize = (int)floor((double)(_maxMaskSize - _minMaskSize) / (double)numMasks);
	if (_maskStepSize % 2 == 1) _maskStepSize -= 1; //maskStepSize has to be an even number
	_maskStepSize = max(4, _maskStepSize); //smallest acceptable step size is 4

	// adjusted estimated of mask max size
	_maxMaskSize = _minMaskSize + numMasks*_maskStepSize;
}

void applyMaskToFullImg(Input * _input, Mask * _mask, IplImage & _imgDst, double & _maxRatio)
{
	int wMask = _mask->getW();
	int hMask = _mask->getH();
	int hSrc = _input->getSize().height;

	//a floating point image for storing intermediate results
	IplImage * imgTemp = cvCreateImage( _input->getSize(), IPL_DEPTH_32F, 1 );

	cvZero(imgTemp);

	for (int y = 0; y < hSrc; y++)
	{
		// check boundary conditions
		int minY = y - (hMask - 1)/2;
		int maxY = y + (hMask - 1)/2;

		if (minY >= 0 && maxY < hSrc)
		{
			// mask width = source width, so only need to do this once for each y (x = 0)
			_input->setROI(cvRect(0, minY, wMask, hMask));

			applyMaskAtY(_input, _mask, y, *imgTemp);

			_input->resetROI();
		}
	}

	//get the location where the match between the source and the mask was the greatest
	cvMinMaxLoc(imgTemp, NULL, &_maxRatio);

	//scale the image of match results based on the max value so that everything is between 0 and 1
	cvScale(imgTemp, imgTemp, 1/_maxRatio);

	//threshold to keep only the locations corresponding to good matches
	cvZero(&_imgDst);
	cvThreshold(imgTemp, &_imgDst, 0.84, 255, CV_THRESH_BINARY);

	cvReleaseImage( &imgTemp );
}

void applyMaskAtY(Input * _input, Mask * _mask, int _y, IplImage & _imgDst)
{
	int wMask = _mask->getW();
	int hMask = _mask->getH();

	IplImage * imgRedMask = cvCreateImage( cvSize(wMask, hMask), IPL_DEPTH_8U, 1 );
	IplImage * imgWhiteMask = cvCreateImage( cvSize(wMask, hMask), IPL_DEPTH_8U, 1 );
	IplImage * imgRedMaskInv = cvCreateImage( cvSize(wMask, hMask), IPL_DEPTH_8U, 1 );
	IplImage * imgWhiteMaskInv = cvCreateImage( cvSize(wMask, hMask), IPL_DEPTH_8U, 1 );

	IplImage * imgAfterMask = cvCreateImage( cvSize(wMask, hMask), IPL_DEPTH_8U, 1 );
	IplImage * imgAfterMaskInv = cvCreateImage( cvSize(wMask, hMask), IPL_DEPTH_8U, 1 );

#ifdef _DEBUG_ALL_MASKS
#ifdef _DEBUG_Y
	if (_y == _DEBUG_Y)
	{
		showBinaryImage("mask_", _mask->getImgInv());
	}
#endif
#endif

	cvZero(imgRedMask);
	cvZero(imgWhiteMask);
	cvZero(imgRedMaskInv);
	cvZero(imgWhiteMaskInv);

	cvCopy(_input->getImgRed(), imgRedMask, _mask->getImg());
	cvCopy(_input->getImgWhite(), imgWhiteMask, _mask->getImgInv());
	cvAdd(imgRedMask, imgWhiteMask, imgAfterMask);

	cvCopy(_input->getImgRed(), imgRedMaskInv, _mask->getImgInv());
	cvCopy(_input->getImgWhite(), imgWhiteMaskInv, _mask->getImg());
	cvAdd(imgRedMaskInv, imgWhiteMaskInv, imgAfterMaskInv);

#ifdef _DEBUG_ALL_MASKS
#ifdef _DEBUG_Y
	if (_y == _DEBUG_Y)
	{
		showBinaryImage("mask small out", imgAfterMask);
	}
#endif
#endif

	IplImage * imgTemp = cvCreateImage( cvSize(_imgDst.width, _imgDst.height), IPL_DEPTH_32F, 1 );
	cvCopy(&_imgDst, imgTemp);
	calculateMatchQuality(imgAfterMask, imgAfterMaskInv, _y, *imgTemp);
	cvCopy(imgTemp, &_imgDst);
	cvReleaseImage(&imgTemp);

	cvReleaseImage(&imgRedMask);
	cvReleaseImage(&imgWhiteMask);
	cvReleaseImage(&imgRedMaskInv);
	cvReleaseImage(&imgWhiteMaskInv);
	cvReleaseImage(&imgAfterMask);
	cvReleaseImage(&imgAfterMaskInv);
}

void calculateMatchQuality(IplImage * _imgAfterMask, IplImage * _imgAfterMaskInv, int _y, IplImage & _imgDst)
{
	int wMask = _imgAfterMask->width;
	int hMask = _imgAfterMask->height;

	/*vector<double> columnSumAfterMask;
	vector<double> columnSumAfterMaskInv;
	for (int x = 0; x < wMask; x++)
	{
		cvSetImageROI(_imgAfterMask, cvRect(x, 0, 1, hMask));
		cvSetImageROI(_imgAfterMaskInv, cvRect(x, 0, 1, hMask));
		columnSumAfterMask.push_back(cvSum(_imgAfterMask).val[0]);
		columnSumAfterMaskInv.push_back(cvSum(_imgAfterMaskInv).val[0]);
		cvResetImageROI(_imgAfterMask);
		cvResetImageROI(_imgAfterMaskInv);
	}*/

	// we have a y-value, so we iterate over all the x-values
	// at each index, take a surrounding area given by mask height x mask height
	// count the number of non-zero pixels (matches between source and mask)
	// divide this number by the total number of pixels within the area to get a measure of match quality
	// repeat with the mask inverse and keep the best of the two results
	// TODO: more efficient to add the values of the newest column and subtract the values of the oldest column
	for (int x = 0; x < wMask; x++)
	{
		int minX = x - (hMask - 1)/2;
		int maxX = x + (hMask - 1)/2;

		if (minX >= 0 && maxX < wMask)
		{	
			// extract column sums corresponding to [minX, maxX]
			//vector<double> subVector1(maxX - minX + 1);
			//vector<double> subVector2(maxX - minX + 1);

			//copy(columnSumAfterMask.begin() + minX, columnSumAfterMask.begin() + maxX + 1, subVector1.begin());
			//double subVectorSum1 = accumulate(subVector1.begin(), subVector1.end(), 0);

			//copy(columnSumAfterMaskInv.begin() + minX, columnSumAfterMaskInv.begin() + maxX + 1, subVector2.begin());
			//double subVectorSum2 = accumulate(subVector2.begin(), subVector2.end(), 0);

			cvSetImageROI(_imgAfterMask, cvRect(minX, 0, hMask, hMask));
			cvSetImageROI(_imgAfterMaskInv, cvRect(minX, 0, hMask, hMask));
			double numNonZero1 = cvSum(_imgAfterMask).val[0];
			double numNonZero2 = cvSum(_imgAfterMaskInv).val[0];

			// ratio = 0.55 if matching a mask to a completely black image
			double ratio1 = numNonZero1 / (double)(hMask * hMask);
			double ratio2 = numNonZero2 / (double)(hMask * hMask);
			double maxRatio = max(ratio1, ratio2);

			cvSet2D(&_imgDst, _y, x, cvScalar(maxRatio));
		}
	}

	cvResetImageROI(_imgAfterMask);
	cvResetImageROI(_imgAfterMaskInv);
}

CvPoint getCenterOfLargestBlob(IplImage * _imgSrc)
{
	CvPoint center = cvPoint(0,0);

	CvMemStorage *mem;
	mem = cvCreateMemStorage(0);
	CvSeq *contours = 0;

	CvSeq *maxContour = 0;
	double maxArea = 0;

    cvFindContours( _imgSrc, mem, &contours, sizeof(CvContour),
        CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

#ifdef _DEBUG_CONTOURS
	IplImage * imgDebug = cvCreateImage( cvGetSize(_imgSrc), IPL_DEPTH_8U, 3 );
	cvZero(imgDebug);
	
	cvNamedWindow( "Contours", 1 );
#endif

	// iterate over the contours to find the biggest
	for (; contours != 0; contours = contours->h_next)
	{
		double area = abs(cvContourArea(contours));
		if (area > 0)
		{
			if (area > maxArea)
			{
				maxArea = area;
				maxContour = contours;
			}

#ifdef _DEBUG_CONTOURS
			CvScalar ext_color = CV_RGB( rand()&255, rand()&255, rand()&255 ); //randomly coloring different contours
			cvDrawContours(imgDebug, contours, ext_color, CV_RGB(0,0,0), -1, CV_FILLED, 8, cvPoint(0,0));
			printf("contour size = %f\n", area);
#endif
		}
	}

	// find the center of the biggest contour
	if (maxContour != NULL)
	{
		CvMoments m;
		cvMoments(maxContour,&m);
		center = cvPoint((int)(m.m10/m.m00), (int)(m.m01/m.m00));
	}

#ifdef _DEBUG_CONTOURS
	cvShowImage( "Contours", imgDebug );
	cvWaitKey(0);
	cvDestroyWindow("Contours");

	cvReleaseImage( &imgDebug );
#endif

	cvReleaseMemStorage( &mem );

	return center;
}

void showBinaryImage(string _winName, IplImage *_imgSrc)
{
	IplImage * imgTemp = cvCreateImage( cvGetSize(_imgSrc), IPL_DEPTH_8U, 1 );
	
	cvCvtScale(_imgSrc, imgTemp, 255);
	cvShowImage(_winName.c_str(), imgTemp);

	cvReleaseImage(&imgTemp);
}