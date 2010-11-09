/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#pragma once

#include <string>
#include <iostream>

#include <opencv\cv.h>
#include <opencv\highgui.h>

#include "ImageProcessor.h"

namespace TUMAugmentedRealityExercise
{
	class VideoWindow
	{
	private:
		std::string name;
		cv::Mat resultBuffer;

		ImageProcessor* processor;
	public:
		VideoWindow(const std::string& name, ImageProcessor* processor);
		~VideoWindow(void);
		
		std::string GetName(void);

		void update(cv::Mat& image);
	};

	class ThresholdTrackbar
	{
	private:
		std::string window;
		std::string name;

		int value;

		ThresholdImageProcessor* processor;

		static void onChange(int value, void* data);
	public:
		ThresholdTrackbar(const std::string& window, ThresholdImageProcessor* processor);
		~ThresholdTrackbar(void) {};
	};
}

