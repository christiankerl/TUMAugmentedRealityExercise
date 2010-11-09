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

#include "MemoryStorage.h"
#include "ImageProcessor.h"
#include "VideoSource.h"
#include "VideoWindow.h"
#include "FPSMonitor.h"

#define ESCAPE_KEY 27
#define C_KEY 99
#define V_KEY 118

using namespace cv;
using namespace TUMAugmentedRealityExercise;

int main(int argc, char* argv[])
{
	bool running = true;
	Mat inBuffer;
	Mat outBuffer;
	
	// setup video input
	VideoSource* source;
	VideoSource* camera = new CameraVideoSource();
	VideoSource* video = NULL;

	if(argc == 2)
		video = new FileVideoSource(std::string(argv[1]));

	source = camera;
	
	FPSMonitor fps;
	fps.SetVideoSourceFPS(source->GetFPS());

	// create dynamic memory
	MemoryStorage memory;

	MarkerContainer markers;

	// create image processors
	GreyscaleImageProcessor grey;
	AdaptiveThresholdImageProcessor adaptive;
	MarkerDetectionImageProcessor marker(&memory, &markers);

	MarkerHighlightImageProcessor highlight(&markers);

	// chain various image processors
	ImageProcessorChain chain1;
	chain1.add(&grey);
	chain1.add(&adaptive);
	chain1.add(&marker);
	
	// create ui
	VideoWindow originWindow("AR-EX1-Origin", &highlight);
	//VideoWindow thresholdWindow("AR-EX1-Threshold", &chain1);
	//VideoWindow adaptiveWindow("AR-EX1-AdaptiveThreshold", &chain2);

	//ThresholdTrackbar tresholdTrackbar(thresholdWindow.GetName(), &threshold);
	
	// start ui event loop
	while(running)
	{
		fps.BeginProcessing();

		// grab next frame
		source->GetNextImage(inBuffer);

		// process and display current frame
		chain1.process(inBuffer, outBuffer);
		
		//thresholdWindow.update(inBuffer);
		originWindow.update(inBuffer);

		// clear memory resources
		memory.Clear();
		markers.clear();

		fps.EndProcessing();

		switch(waitKey(fps.GetMsUntilNextFrame()))
		{
		case C_KEY:
			std::cout << "c key pressed -> switching to camera mode" << std::endl;
			source = camera;

			fps.SetVideoSourceFPS(source->GetFPS());
			break;
		case V_KEY:
			std::cout << "v key pressed -> switching to video mode" << std::endl;
			if(video != NULL)
				source = video;

			fps.SetVideoSourceFPS(source->GetFPS());
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