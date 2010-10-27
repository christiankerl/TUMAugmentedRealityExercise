/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#include "VideoWindow.h"

namespace TUMAugmentedRealityExercise
{
	VideoWindow::VideoWindow(const std::string& name, cv::VideoCapture* video, ImageProcessor* processor)
	{
		this->name = name;
		this->video = video;
		this->processor = processor;

		cvNamedWindow(this->name.c_str());
	}

	VideoWindow::~VideoWindow(void)
	{
		cvDestroyWindow(this->name.c_str());
	}

	void VideoWindow::update()
	{
		this->video->retrieve(this->buffer);

		cv::Mat result;

		this->processor->process(this->buffer, result);

		CvMat copy = result;

		cvShowImage(this->name.c_str(), &copy);
	}

	ThresholdTrackbar::ThresholdTrackbar(const std::string& window, ThresholdImageProcessor* processor)
	{
		this->window = window;
		this->name = "Threshold";
		this->processor = processor;
		this->value = (int)processor->threshold;

		cvCreateTrackbar2(this->name.c_str(), this->window.c_str(), &this->value, 255, &ThresholdTrackbar::onChange, this);
	}

	void ThresholdTrackbar::onChange(int value, void* data)
	{
		ThresholdTrackbar* trackbar = (ThresholdTrackbar*) data;

		trackbar->processor->threshold = value;
	}
}