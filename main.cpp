/*************************************************************************
   "Where's Senor Waldos?"
   Miovision Programming Contest
 
   Copyright Maxim Reznitskii
   Version 1: Dec. 21, 2007
   Version 2: Aug. 10, 2020

   Created with OpenCV 1.0 and Microsoft Visual Studio 2008
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
#include <fstream>

using namespace std;

string FOLDER = "Images/";
string INPUT_FILE = FOLDER + "input.txt";
string OUTPUT_FILE = FOLDER + "output.txt";
#define PRINT_TO_OUT_FILE true

#define _DEBUG_FILTER false
#define _DEBUG_INPUT true

int main(int argc, char* argv[])
{
	Input * input;

	string line, output;
	char entry[20];
	CvPoint center;

	if (_DEBUG_INPUT)
		cvNamedWindow("src", CV_WINDOW_AUTOSIZE);

	//read provided text file to get list of images to process
	ifstream infile (INPUT_FILE.c_str(), ios_base::in);
	while (getline(infile, line, ','))
	{
		//create images
		input = new Input(FOLDER + line + ".jpg", _DEBUG_FILTER);

		if (_DEBUG_INPUT)
		{
			printf("Loaded ");
			printf((FOLDER + line + ".jpg\n").c_str());

			input->showBgr("src");
		}

		time_t before = time(0); 

		center = findWaldo(input);

		time_t after = time(0); 
		double duration = difftime(after, before);

		IplImage * imgTemp = cvCloneImage(input->getImgBgr());

		//draw a bullseye
		cvCircle( imgTemp, center, 5, CV_RGB(0, 0, 255), -1 );
		cvCircle( imgTemp, center, 12, CV_RGB(0, 0, 255), 4);
		cvCircle( imgTemp, center, 24, CV_RGB(0, 0, 255), 4);

		if (_DEBUG_INPUT)
		{
			printf("Done: (%d, %d) (%.0f sec)\n", center.x, center.y, duration); 

			cvShowImage("src", imgTemp);
			cvWaitKey(0);
		}

		//save the image
		cvSaveImage( (FOLDER + line + "_final.jpg").c_str(), imgTemp );

		cvReleaseImage(&imgTemp);

		sprintf_s(entry, "(%d,%d),", center.x, center.y);

		if (PRINT_TO_OUT_FILE)
			output += entry;
	}

	if (PRINT_TO_OUT_FILE)
	{
		//print to output file
		ofstream outfile (OUTPUT_FILE.c_str(), ios_base::out);
		outfile.write(output.c_str(), output.length() - 1);
		outfile.close();
	}

	//close opened files
	infile.close();

	delete input;

	if (_DEBUG_INPUT)
		cvDestroyWindow("src");

	return 0;
}