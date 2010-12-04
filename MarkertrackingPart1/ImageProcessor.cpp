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

	void ResizeImageProcessor::process(cv::Mat& input, cv::Mat& output)
	{
		cv::resize(input, output, cv::Size(), this->factor, this->factor, 0);
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
		cv::adaptiveThreshold(input, output, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 55, 5);
	}

	MarkerDetectionImageProcessor::MarkerDetectionImageProcessor(const MemoryStorage* memory, MarkerContainer* markers) : memory(memory), markers(markers)
	{
	}

	void MarkerDetectionImageProcessor::process(cv::Mat& input, cv::Mat& output)
	{
		cv::Mat image = input.clone();
		CvSeq* contours;
		
		cvFindContours(&(CvMat)image, this->memory->GetPointer(), &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
		
		for(; contours; contours = contours->h_next)
		{
			CvRect boundingbox = cvBoundingRect(contours, 0);

			if(boundingbox.width < 35 || boundingbox.height < 35 || boundingbox.width > input.cols - 10)
			{
				continue;
			}

			CvSeq* rectangle = cvApproxPoly(contours, sizeof(CvContour), this->memory->GetPointer(), CV_POLY_APPROX_DP, cvContourPerimeter(contours) * 0.02, 0);

			if(rectangle->total == 4)
			{
				Marker marker((std::vector<cv::Point>) cv::Seq<cv::Point>(rectangle));

				for(int a = 0; a < marker.Corners.size(); a++)
				{
					cv::Point current = marker.Corners[a];
					cv::Point next = marker.Corners[(a + 1) < marker.Corners.size() ? a + 1 : 0];

					// interpolate 6 points between 2 corner points
					cv::Point2d line(next.x - current.x, next.y - current.y);
					cv::Point2d intermediate(current.x, current.y);

					cv::Point2d increment = (1.0 / 7.0) * line;

					double halfStripeWidth = std::max(0.4 * length(increment), 2.5);						

					cv::Point2d direction = normalize(line);
					cv::Point2d normal = normalize(cv::Point2d(line.y, -line.x)); 

					for(int b = 0; b < 6; b++)
					{
						intermediate += increment;

						cv::Point2d stripeTopLeftCorner  = (intermediate + direction) + (1.0 * normal * halfStripeWidth);
						cv::Point2d stripeTopRightCorner  = (intermediate + direction) + (-1.0 * normal * halfStripeWidth);
						cv::Point2d stripeBottomLeftCorner  = (intermediate - direction) + (1.0 * normal * halfStripeWidth);
						cv::Point2d stripeBottomRightCorner  = (intermediate - direction) + (-1.0 * normal * halfStripeWidth);

						MarkerStripe stripe;
						stripe.Center = intermediate;
						stripe.IterationNormalX = -1.0 * normal;
						stripe.IterationNormalY = -1.0 * direction;

						stripe.Width = 2 * halfStripeWidth;
						stripe.Height = 3;

						stripe.Corners.push_back(stripeTopLeftCorner);
						stripe.Corners.push_back(stripeTopRightCorner);
						stripe.Corners.push_back(stripeBottomRightCorner);
						stripe.Corners.push_back(stripeBottomLeftCorner);

						stripe.SampleFromImage(input);
						stripe.CalculateSubPixelCenter();

						marker.Stripes.push_back(stripe);
					}
				}

				marker.CalculateSubPixelCorners();

				if(marker.SampleFromImageAndDecode(input))
				{
					marker.EstimatePose();

					this->markers->push_back(marker);
				}
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
		
		const cv::Point* first = &marker.Corners.front();

		cv::polylines(image, &first, RectangleCorners, 1, true, cv::Scalar(0, 0, 255), 1);

		int r = 0;

		for(int a = 0; a < marker.SubPixelCorners.size(); a++)
		{
			cv::circle(image, marker.SubPixelCorners[a], 2, cv::Scalar(255 - r, 0, r), 2);
			r += 60;
		}
		
		for(int a = 0; a < marker.Corners.size(); a++)
		{
			cv::circle(image, marker.Corners[a], 1, cv::Scalar(0, 255, 0), 1);
		}

		std::vector<cv::Point> stripeCorners;

		for(int a = 0; a < marker.Stripes.size(); a++)
		{
			stripeCorners.assign(marker.Stripes[a].Corners.begin(), marker.Stripes[a].Corners.end());
			const cv::Point* firstStripeCorner = &stripeCorners.front();

			cv::polylines(image, &firstStripeCorner, RectangleCorners, 1, true, cv::Scalar(a*10, 255 - a * 10, 0));
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