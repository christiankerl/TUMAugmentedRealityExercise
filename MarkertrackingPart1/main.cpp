/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#include <ostream>
#include <iostream>

#include <opencv\cv.h>
#include <opencv\highgui.h>

#include "ImageProcessor.h";
#include "VideoSource.h";
#include "VideoWindow.h";

#define ESCAPE_KEY 27
#define C_KEY 99
#define V_KEY 118

using namespace cv;
using namespace TUMAugmentedRealityExercise;

int main(int argc, char* argv[])
{
	bool running = true;
	Mat buffer;

	// setup video input
	VideoSource* source;
	VideoSource* camera = new CameraVideoSource();
	VideoSource* video = NULL;

	if(argc == 2)
		video = new FileVideoSource(std::string(argv[1]));

	source = camera;
	
	// create image processors
	NullImageProcessor empty;

	GreyscaleImageProcessor grey;

	ThresholdImageProcessor threshold;
	threshold.threshold = 104;

	AdaptiveThresholdImageProcessor adaptive;

	// chain various image processors
	ImageProcessorChain chain1;
	chain1.add(&grey);
	chain1.add(&threshold);

	ImageProcessorChain chain2;
	chain2.add(&grey);
	chain2.add(&adaptive);

	// create ui
	VideoWindow originWindow("AR-EX1-Origin", &empty);
	VideoWindow thresholdWindow("AR-EX1-Threshold", &chain1);
	VideoWindow adaptiveWindow("AR-EX1-AdaptiveThreshold", &chain2);

	ThresholdTrackbar tresholdTrackbar(thresholdWindow.GetName(), &threshold);

	// start ui event loop
	while(running)
	{
		// grab next frame
		source->GetNextImage(buffer);

		// process and display current frame
		originWindow.update(buffer);
		thresholdWindow.update(buffer);
		adaptiveWindow.update(buffer);
		
		switch(waitKey(30))
		{
		case C_KEY:
			std::cout << "c key pressed -> switching to camera mode" << std::endl;
			source = camera;
			break;
		case V_KEY:
			std::cout << "v key pressed -> switching to video mode" << std::endl;
			if(video != NULL)
				source = video;
			break;
		case ESCAPE_KEY:
			running = false;
			break;
		}
	}

	delete camera;

	if(video != NULL)
		delete video;

	return 0;
}