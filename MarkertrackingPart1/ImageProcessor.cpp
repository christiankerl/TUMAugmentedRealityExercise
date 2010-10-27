/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#include "ImageProcessor.h"

namespace TUMAugmentedRealityExercise
{
	void NullImageProcessor::process(cv::Mat& input, cv::Mat& output)
	{
		output = input;
	}

	void GreyscaleImageProcessor::process(cv::Mat& input, cv::Mat& output)
	{
		cv::cvtColor(input, output, CV_BGR2GRAY);
	}

	void ThresholdImageProcessor::process(cv::Mat& input, cv::Mat& output)
	{
		cv::threshold(input, output, this->threshold, 255, CV_THRESH_BINARY);
	}

	void AdaptiveThresholdImageProcessor::process(cv::Mat& input, cv::Mat& output)
	{
		cv::adaptiveThreshold(input, output, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 27, 5);
	}

	ImageProcessorChain::ImageProcessorChain(void) : processors()
	{
	}

	ImageProcessorChain::~ImageProcessorChain(void)
	{
		this->processors.clear();
	}

	void ImageProcessorChain::add(ImageProcessor* processor)
	{
		this->processors.push_back(processor);
	}

	void ImageProcessorChain::process(cv::Mat& input, cv::Mat& output)
	{
		cv::Mat lastResult = input;

		for (std::vector<ImageProcessor*>::iterator it = this->processors.begin(); it!=this->processors.end(); ++it) 
		{
			(*it)->process(lastResult, output);

			lastResult = output.clone();
		}
	}
}