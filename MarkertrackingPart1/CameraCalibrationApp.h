/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#pragma once

#include <iostream>

#include <opencv\cv.h>
#include <opencv\highgui.h>

#include "VideoSource.h"
#include "VideoWindow.h"
#include "FPSMonitor.h"

#include "ChessboardCameraCalibrator.h"

namespace TUMAugmentedRealityExercise
{
	class CameraCalibrationApp
	{
	private:
		bool running;
		bool sampleChessboardImage;
		bool sampleChessboardImageFolder;
		bool calculateCalibration;

		cv::Mat buffer;

		FPSMonitor fps;

		VideoSource* video;
		VideoWindow window;

		NullImageProcessor processor;

		ChessboardCameraCalibrator calibrator;

		void HandleKeyboardInput(int key);
	public:
		CameraCalibrationApp(void);
		~CameraCalibrationApp(void);

		int Run(int argc, char* argv[]);
	};
}

