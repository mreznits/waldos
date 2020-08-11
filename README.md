# waldos

   "Where's Senor Waldos?"
   Miovision Programming Contest
 
   Copyright Maxim Reznitskii
   Version 1: Dec. 21, 2007
   Version 2: Aug. 10, 2020
   
   Project files:
   - main.cpp = Program entry point
   - Input.h = Class for processing an input image
   - Mask.h = Class for creating a mask of fixed dimensions
   - Waldos.h & Waldos.cpp = Core algorithm functions for finding Waldos
   - Images - Directory for input and output images
   - sampleDebugOutput.jpg - Image of sample debug output of the program
   - waldos-Debug.exe - Debug executable of the program
   - waldos-Release.exe - Release executable of the program
   - LICENSE - GNU General Public License
   - waldos.sln & waldos.vcproj - Visual Studio 2008 project files
   
   Abstract of algorithm:
   - Idea #1: Waldo may partially occluded, he might wear different hats, but his face and shirt 
		are always the same and always visible -> search the image for his shirt
   - Idea #2: Waldo's shirt is always red and white -> apply colour-based filters
   - Idea #3: Waldo's shirt is always striped -> search for it using a mask of stripes
   - Idea #4: In the images, Waldo is always vertical -> shirt stripes are always horizontal
   - Idea #5: Since the mask is invariant along x, can make it the entire width of the image