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
		cv::adaptiveThreshold(input, output, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 55, 20);
	}

	MarkerDetectionImageProcessor::MarkerDetectionImageProcessor(const MemoryStorage* memory, MarkerContainer* markers) : memory(memory), markers(markers)
	{
	}

	void MarkerDetectionImageProcessor::process(cv::Mat& input, cv::Mat& output)
	{
		CvMat image = input;
		CvSeq* contours;

		cvFindContours(&image, this->memory->GetPointer(), &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

		for(; contours; contours = contours->h_next)
		{
			CvSeq* rectangle = cvApproxPoly(contours, sizeof(CvContour), this->memory->GetPointer(), CV_POLY_APPROX_DP, cvContourPerimeter(contours) * 0.02, 0);

			if(rectangle->total == 4)
			{
				CvRect boundingbox = cvBoundingRect(contours, 0);

				if(boundingbox.width < 15 || boundingbox.height < 15) continue;

				this->markers->push_back(cv::Seq<cv::Point>(rectangle));
			}
		}
	}

	MarkerHighlightImageProcessor::MarkerHighlightImageProcessor(const MarkerContainer* markers) : markers(markers)
	{

	}

	void MarkerHighlightImageProcessor::process(cv::Mat& input, cv::Mat& output)
	{
		output = input;

		// iterate over all markers
		for(MarkerContainer::const_iterator it = this->markers->begin(); it != this->markers->end(); ++it)
		{
			this->HighlightMarker(output, *it);
		}
	}
	
	void MarkerHighlightImageProcessor::HighlightMarker(cv::Mat& image, const Marker& marker)
	{
		int RectangleCorners[] = { 4 };

		const cv::Point* first = &marker.front();

		cv::polylines(image, &first, RectangleCorners, 1, true, cv::Scalar(0, 0, 255), 2);

		for(int a = 0; a < marker.size(); a++)
		{
			cv::Point current = marker[a];
			cv::Point next = marker[(a + 1) < marker.size() ? a + 1 : 0];

			// draw circle around corner point
			cv::circle(image, current, 2, cv::Scalar(0, 255, 0), 2);

			// interpolate 6 points between 2 corner points
			double x_increment = (next.x - current.x) / 7.0;
			double y_increment = (next.y - current.y) / 7.0;

			cv::Point2d intermediate(current.x, current.y);

			for(int b = 0; b < 6; b++)
			{
				intermediate.x += x_increment;
				intermediate.y += y_increment;

				cv::circle(image, intermediate, 1, cv::Scalar(255, 0, 0), 1);
			}
		}
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

		for (std::vector<ImageProcessor*>::iterator it = this->processors.begin(); it != this->processors.end(); ++it) 
		{
			(*it)->process(lastResult, output);

			lastResult = output.clone();
		}
	}
}