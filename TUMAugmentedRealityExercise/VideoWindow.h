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

#include "ImageProcessor.h";

namespace TUMAugmentedRealityExercise
{
	class VideoWindow
	{
	private:
		std::string name;

		cv::Mat buffer;
		cv::VideoCapture* video;

		ImageProcessor* processor;
	public:
		VideoWindow(const std::string& name, cv::VideoCapture* video, ImageProcessor* processor);
		~VideoWindow(void);
		
		void update();
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

