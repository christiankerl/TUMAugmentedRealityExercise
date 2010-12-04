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

#include "MemoryStorage.h"
#include "Marker.h"

namespace TUMAugmentedRealityExercise
{
	class ImageProcessor
	{
	public:
		virtual void process(cv::Mat& input, cv::Mat& output) = 0;
	};

	class NullImageProcessor : public ImageProcessor
	{
	public:
		NullImageProcessor(void) {};
		~NullImageProcessor(void) {};

		void process(cv::Mat& input, cv::Mat& output);
	};

	class ResizeImageProcessor : public ImageProcessor
	{
	private:
		double factor;
	public:
		ResizeImageProcessor(double factor) : factor(factor) {};
		~ResizeImageProcessor(void) {};

		void process(cv::Mat& input, cv::Mat& output);
	};

	class GreyscaleImageProcessor : public ImageProcessor
	{
	public:
		GreyscaleImageProcessor(void) {};
		~GreyscaleImageProcessor(void) {};

		void process(cv::Mat& input, cv::Mat& output);
	};

	class ThresholdImageProcessor : public ImageProcessor
	{
	public:
		double threshold;

		ThresholdImageProcessor(void) {};
		~ThresholdImageProcessor(void) {};

		void process(cv::Mat& input, cv::Mat& output);
	};

	class AdaptiveThresholdImageProcessor : public ImageProcessor
	{
	public:
		AdaptiveThresholdImageProcessor(void) {};
		~AdaptiveThresholdImageProcessor(void) {};

		void process(cv::Mat& input, cv::Mat& output);
	};

	class MarkerDetectionImageProcessor : public ImageProcessor
	{
	private:
		const MemoryStorage* memory;
		MarkerContainer* markers;

	public:
		MarkerDetectionImageProcessor(const MemoryStorage* memory, MarkerContainer* markers);
		~MarkerDetectionImageProcessor(void) {};

		void process(cv::Mat& input, cv::Mat& output);
	};

	class MarkerHighlightImageProcessor : public ImageProcessor
	{
	private:
		const MarkerContainer* markers;

		void HighlightMarker(cv::Mat& image, const Marker& marker);
	public:
		MarkerHighlightImageProcessor(const MarkerContainer* markers);
		~MarkerHighlightImageProcessor(void) {};

		void process(cv::Mat& input, cv::Mat& output);
	};

	class ImageProcessorChain : public ImageProcessor
	{
	private:
		std::vector<ImageProcessor*> processors;
	public:
		ImageProcessorChain(void);
		~ImageProcessorChain(void);

		void add(ImageProcessor* processor);

		void process(cv::Mat& input, cv::Mat& output);
	};
}