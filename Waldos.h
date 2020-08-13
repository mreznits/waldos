/*************************************************************************
    "Where's Senor Waldos?"
    Miovision Programming Contest
 
    Copyright Maxim Reznitskii
    Version 1: Dec. 21, 2007
    Version 2: Aug. 10, 2020

    Created with OpenCV 1.0 and Microsoft Visual Studio 2008

    Waldos.h - Core algorithm functions for finding Waldos
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

#ifndef _WALDOS_H
#define _WALDOS_H

#include "Mask.h"
#include "Input.h"

#include <string>

#include "cv.h"
#include "highgui.h" 
#include <cxcore.h>

//#define _DEBUG_ALL_MASKS
//#define _DEBUG_X 239 //good for level1.img
//#define _DEBUG_Y 246 //good for level1.img
//#define _DEBUG_X 294 //good for level2.img
//#define _DEBUG_Y 161 //good for level2.img
//#define _DEBUG_X 181 //good for level3.1.img
//#define _DEBUG_Y 159 //good for level3.1.img
//#define _DEBUG_X 525 //good for scene3.4.img
//#define _DEBUG_Y 477 //good for scene3.4.img
//#define _DEBUG_CONTOURS

//-----------------------------------------------------------------------------------------------------
// Find Senor Waldos!
// Idea #1: Waldo may partially occluded, he might wear different hats, but his face and shirt 
//      are always the same and always visible -> search the image for his shirt
// Idea #2: Waldo's shirt is always red and white -> apply colour-based filters
// Idea #3: Waldo's shirt is always striped -> search for it using a mask of stripes
// Idea #4: In the images, Waldo is always vertical -> shirt stripes are always horizontal
// Idea #5: Since the mask is invariant along x, can make it the entire width of the image
//
// [input] _input               = input object
// [input] _bDebug              = debug flag
// [output, return]             = center point 
//-----------------------------------------------------------------------------------------------------
CvPoint findWaldo(Input * _input, bool _bDebug);

//-----------------------------------------------------------------------------------------------------
// Try sliding different-sized masks across the source image.
// For each mask, we get: 
// - an image showing locations where the match between the mask and the source image was good
// - an indicator of the mask quality
// Choose the best one. 
//
// [input] _input               = input object
// [input] _bDebug              = debug flag
// [output, ref] _imgDst        = binary match location image of the best mask
//-----------------------------------------------------------------------------------------------------
void findMaskMatchLoc(Input * _input, bool _bDebug, IplImage & _imgDst);

//-----------------------------------------------------------------------------------------------------
// Get the optimal parameters for mask dimensions based on the dimensions of the input image.
//
// [input] _iWidth              = width of the input image
// [input] _iHeight             = height of the input image
// [output, ref] _iMinMaskSize  = minimum size of the mask
// [output, ref] _iMaxMaskSize  = maximum size of the mask
// [output, ref] _iMaskStepSize = mask step size
//
//-----------------------------------------------------------------------------------------------------
void getOptimalMaskParams(int _iWidth, int _iHeight, int & _iMinMaskSize, int & _iMaxMaskSize, int & _iMaskStepSize);

//-----------------------------------------------------------------------------------------------------
// Slide the mask across the source image. Result is a floating point image, showing a match quality 
//      value at each pixel index.  
// Threshold this image to retain only the good matches. 
//
// [input] _input               = input object
// [input] _mask                = mask object
// [output, ref] _imgDst        = image (black / white) showing locations where there is a good match  
//                                  between the source and the mask
// [output, ref] _dMaxRatio     = value corresponding to the best match location in the image 
//-----------------------------------------------------------------------------------------------------
void applyMaskToFullImg(Input * _input, Mask * _mask, IplImage & _imgDst, double & _dMaxRatio);

//-----------------------------------------------------------------------------------------------------
// At each y-location: 
// - Apply mask to image of red pixels
// - Apply inverse mask to image of white pixels
// - Add the two results together (1)
// - Apply inverse mask to image of red pixels
// - Apply mask to image of white pixels
// - Add the two results together (2)
// - Evaluate the quality of the match (consider both (1) and (2) and keep the best of the two)
// [Note: This is the place where an AI algorithm could be used but, in our case, the problem can be  
//      solved with a simpler solution.]
//
// [input] _input               = input object
// [input] _mask                = mask object
// [input] _iY                  = y-location in the source image that we are working with 
// [output, ref] _imgDst        = image (floating point) showing the quality of the match between the   
//                                  source and the mask
//-----------------------------------------------------------------------------------------------------
void applyMaskAtY(Input * _input, Mask * _mask, int _iY, IplImage & _imgDst);

//-----------------------------------------------------------------------------------------------------
// Calculate how well the mask matched the source image.
//
// [input] _imgAfterMask        = binary image showing result of mask applied to source
// [input] _imgAfterMaskInv     = binary image showing result of mask inverse applied to source
// [input] _iY                  = y-location in the source image that we are working with 
// [output, ref] _imgDst        = image (floating point) showing locations where there is a good match  
//                                  between the source and the mask
//-----------------------------------------------------------------------------------------------------
void calculateMatchQuality(IplImage * _imgAfterMask, IplImage * _imgAfterMaskInv, int _iY, IplImage & _imgDst);

//-----------------------------------------------------------------------------------------------------
// Find the center of the biggest blog in the image.
//
// [input] _imgSrc              = source image (black / white)
// [output, return]             = center point of the biggest blob
//-----------------------------------------------------------------------------------------------------
CvPoint getCenterOfLargestBlob(IplImage * _imgSrc);

//-----------------------------------------------------------------------------------------------------
// Function for debugging. Convert image with values 0/1 to 0/255 and display it. 
//
// [input] _sWinName            = name of the debug window
// [input] _imgSrc              = source image
//-----------------------------------------------------------------------------------------------------
void showBinaryImage(std::string _sWinName, IplImage *_imgSrc);

#endif
