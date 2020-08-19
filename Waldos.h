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
// Finds Senor Waldos!
// Idea #1: Waldos may be partially occluded, he might wear different hats, but his face and shirt 
//      are always the same and always visible -> search the image for his shirt
// Idea #2: Waldos' shirt is always red and white -> apply colour-based filters
// Idea #3: Waldos' shirt is always striped -> search for it using a mask of stripes
// Idea #4: In the images, Waldos is always vertical -> shirt stripes are always horizontal
// Idea #5: Because the mask is invariant along x, can make it the entire width of the image
//
// Parameters:
//
// _input                       Type: Input object [input]
//                              Contains the original input image, a binary image of red pixel 
//                              locations, and a binary image of white pixel locations.
//
// _bDebug                      Type: boolean [input]
//                              Debug flag. If true, prints out the dimensions of the best mask and
//                              displays the result of applying this mask. 
//
// Returns:
//
// CvPoint                      Waldos' location in the image.
//
// Example:
// 
// _input                       Input("Examples/level2.jpg")
// return value                 CvPoint(290,175)
//
// "Examples/findWaldos.jpg" shows the preceding center point overlayed on the input image
//
//-----------------------------------------------------------------------------------------------------
CvPoint findWaldos(Input * _input, bool _bDebug);

//-----------------------------------------------------------------------------------------------------
// Tries sliding different-sized masks across the source image.
// For each mask, gets: 
// - An image showing locations where the match between the mask and the source image was good
// - An indicator of the mask quality
// Chooses the best one. 
//
// Parameters:
//
// _input                       Type: Input object [input]
//                              Contains the original input image, a binary image of red pixel 
//                              locations, and a binary image of white pixel locations.
//
// _bDebug                      Type: boolean [input]
//                              Debug flag. If true, prints out the dimensions of the best mask and
//                              displays the result of applying this mask. 
//
// _imgDst                      Type: IplImage [output only]
//                              Depth: 8U [expected image values: 0/1]
//                              Shows match locations of the best mask.
//
// Example:
// 
// _input                       Input("Examples/level2.jpg")
// _imgDst                      "Examples/findMaskMatchLoc.jpg"
//
//-----------------------------------------------------------------------------------------------------
void findMaskMatchLoc(Input * _input, bool _bDebug, IplImage & _imgDst);

//-----------------------------------------------------------------------------------------------------
// Gets the optimal parameters for mask dimensions based on the dimensions of the input image.
//
// Parameters:
//
// _iWidth                      Type: integer [input]
//                              Width of the input image.
//
// _iHeight                     Type: integer [input]
//                              Height of the input image.
// 
// _iMinMaskSize                Type: integer [output only]
//                              Minimum size of the mask.
// 
// _iMaxMaskSize                Type: integer [output only]
//                              Maximum size of the mask.
// 
// _iMaskStepSize               Type: integer [output only]
//                              Mask step size.
//
// Example:
// 
// _iWidth                      512
// _iHeight                     384
// _iMinMaskSize                9
// _iMaxMaskSize                33
// _iMaskStepSize               4
//
// Mask sizes will be 9x9, 13x13, 17x17, 21x21, 25x25, 29x29, and 33x33
//
//-----------------------------------------------------------------------------------------------------
void getOptimalMaskParams(int _iWidth, int _iHeight, int & _iMinMaskSize, int & _iMaxMaskSize, int & _iMaskStepSize);

//-----------------------------------------------------------------------------------------------------
// Slides the mask across the source image. 
// Result is a floating point image, showing a match quality value at each pixel index.
// Thresholds the floating point image to retain only the good matches. 
//
// Parameters:
//
// _input                       Type: Input object [input]
//                              Contains the original input image, a binary image of red pixel 
//                              locations, and a binary image of white pixel locations.
//
// _mask                        Type: Mask object [input]
//
// _imgDst                      Type: IplImage [output only]
//                              Depth: 8U [expected image values: 0/1]
//                              Shows locations where there is a good match between the source 
//                              and the given mask.
//
// _dMaxRatio                   Type: double [output only]
//                              Value corresponding to the best match location in the image.
//
// Example:
// 
// _input                       Input("Examples/level2.jpg")
// _mask                        Mask(width = 17, height = 17)
// _imgDst                      "Examples/applyMaskToFullImg_17x17.jpg" 
// _dMaxRatio                   0.68
//
// _input                       Input("Examples/level2.jpg")
// _mask                        Mask(width = 25, height = 25)
// _imgDst                      "Examples/applyMaskToFullImg_25x25.jpg" 
// _dMaxRatio                   0.88
//
// _input                       Input("Examples/level2.jpg")
// _mask                        Mask(width = 33, height = 33)
// _imgDst                      "Examples/applyMaskToFullImg_33x33.jpg" 
// _dMaxRatio                   0.68
//
//-----------------------------------------------------------------------------------------------------
void applyMaskToFullImg(Input * _input, Mask * _mask, IplImage & _imgDst, double & _dMaxRatio);

//-----------------------------------------------------------------------------------------------------
// At each y-location: 
//      - Applies mask to image of red pixels
//      - Applies inverse mask to image of white pixels
//      - Adds the two results together (1)
//      - Applies inverse mask to image of red pixels
//      - Applies mask to image of white pixels
//      - Adds the two results together (2)
//      - Evaluates the quality of the match (consider both (1) and (2) and keep the best of the two)
// [Note: This is the place where an AI algorithm could be used but. Here, however, the problem can   
//      be solved with a simpler solution.]
//
// Parameters:
//
// _input                       Type: Input object [input]
//                              Contains the original input image, a binary image of red pixel 
//                              locations, and a binary image of white pixel locations.
//
// _mask                        Type: Mask object [input]
//
// _iY                          Type: integer [input]
//                              Y-location in the source image that we are working with.
//
// _imgDst                      Type: IplImage [output only]
//                              Depth: 32F
//                              Shows the quality of the match between the source and the mask.
//
//-----------------------------------------------------------------------------------------------------
void applyMaskAtY(Input * _input, Mask * _mask, int _iY, IplImage & _imgDst);

//-----------------------------------------------------------------------------------------------------
// Calculates how well the mask matched the source image.
//
// Parameters:
//
// _imgAfterMask                Type: IplImage [input]
//                              Depth: 8U [expected image values: 0/1]
//                              Shows the result of the mask applied to the source.
//
// _imgAfterMaskInv             Type: IplImage [input]
//                              Depth: 8U [expected image values: 0/1]
//                              Shows the result of the mask inverse applied to the source.
//
// _iY                          Type: integer [input]
//                              Y-location in the source image that we are working with.
//
// _imgDst                      Type: IplImage [output only]
//                              Depth: 32F
//                              Shows locations where there is a good match between the 
//                              source and the mask.
//
//-----------------------------------------------------------------------------------------------------
void calculateMatchQuality(IplImage * _imgAfterMask, IplImage * _imgAfterMaskInv, int _iY, IplImage & _imgDst);

//-----------------------------------------------------------------------------------------------------
// Finds the center of the biggest blog in the image.
//
// Parameters:
//
// _imgSrc                      Type: IplImage [input]
//                              Depth: 8U [expected image values: 0/1]
//                              Source image.
// 
// Returns:
// 
// CvPoint                      Center point of the biggest blob.
//
// Example:
// 
// _imgSrc                      "Examples/findMaskMatchLoc.jpg"
// return value                 CvPoint(290,175)
//
//-----------------------------------------------------------------------------------------------------
CvPoint getCenterOfLargestBlob(IplImage * _imgSrc);

//-----------------------------------------------------------------------------------------------------
// Converts image values from 0/1 to 0/255. Displays the image. Function for debugging. 
//
// Parameters:
// 
// _sWinName                    Type: string [input]
//                              Name of the debug window.
//
// _imgSrc                      Type: IplImage [input]
//                              Depth: 8U [expected image values: 0/1]
//                              Source image.
//
//-----------------------------------------------------------------------------------------------------
void showBinaryImage(std::string _sWinName, IplImage *_imgSrc);

#endif
