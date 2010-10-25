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
#include "VideoWindow.h";

using namespace cv;
using namespace TUMAugmentedRealityExercise;

int main()
{
	int ESCAPE_KEY = 27;

	VideoCapture video;

	std::cout << "Opening default video device" << std::endl;

	if(!video.open(0))
	{
		std::cout << "Error opening default video device" << std::endl;
		return 0;
	}
	
	// create image processors
	NullImageProcessor empty;

	GreyscaleImageProcessor grey;

	ThresholdImageProcessor threshold;
	threshold.threshold = 104;

	// chain various image processors
	ImageProcessorChain chain1;
	chain1.add(&grey);
	chain1.add(&threshold);

	// create ui
	VideoWindow originWindow("myVideo-Origin", &video, &empty);
	VideoWindow thresholdWindow("myVideo-Threshold", &video, &chain1);

	ThresholdTrackbar tresholdTrackbar("myVideo-Threshold", &threshold);

	// start ui event loop
	for(;;)
	{
		// grab next frame
		video.grab();

		// process and display current frame
		originWindow.update();
		thresholdWindow.update();

		if(waitKey(30) == ESCAPE_KEY)
		{
			break;
		}
	}

	return 0;
}