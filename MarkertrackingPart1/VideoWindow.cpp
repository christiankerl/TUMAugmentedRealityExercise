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
	VideoWindow::VideoWindow(const std::string& name, ImageProcessor* processor)
	{
		this->name = name;
		this->processor = processor;

		cvNamedWindow(this->name.c_str());
	}

	VideoWindow::~VideoWindow(void)
	{
		cvDestroyWindow(this->name.c_str());
	}

	std::string VideoWindow::GetName(void)
	{
		return this->name;
	}

	void VideoWindow::update(cv::Mat& image)
	{
		this->processor->process(image, resultBuffer);

		cvShowImage(this->name.c_str(), &((CvMat) resultBuffer));
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