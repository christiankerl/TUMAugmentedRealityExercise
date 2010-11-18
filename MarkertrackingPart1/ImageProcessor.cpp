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
	double length(cv::Point2d vector)
	{
		return sqrt(vector.x*vector.x + vector.y*vector.y);
	}

	cv::Point2d normalize(cv::Point2d vector)
	{
		double norm =  1.0 / length(vector);

		return cv::Point2d(norm * vector.x, norm * vector.y);
	}

	MarkerStripe::MarkerStripe(void) : Buffer(NULL), Width(-1), Height(-1)
	{
	}

	MarkerStripe::MarkerStripe(const MarkerStripe& copy) :
		Buffer(copy.Buffer),

		Center(copy.Center),
		SubPixelCenter(copy.SubPixelCenter),
		Corners(copy.Corners),

		Width(copy.Width),
		Height(copy.Height),

		IterationNormalX(copy.IterationNormalX),
		IterationNormalY(copy.IterationNormalY)
	{

	}

	MarkerStripe::~MarkerStripe(void) 
	{
	}

	int MarkerStripe::SampleSubPixelFromImage(const cv::Mat image, cv::Point2d p)
	{
		int x = int( floorf ( p.x ) );
		int y = int( floorf ( p.y ) );

		if ( x < 0 || x >= image.cols  - 1 || y < 0 || y >= image.rows - 1 )
			return 127;
		
		int dx = int ( 256 * ( p.x - floorf ( p.x ) ) );
		int dy = int ( 256 * ( p.y - floorf ( p.y ) ) );

		unsigned char* i = ( unsigned char* ) ( ( image.data + y * image.step ) + x );
		int a = i[ 0 ] + ( ( dx * ( i[ 1 ] - i[ 0 ] ) ) >> 8 );
		i += image.step;
		int b = i[ 0 ] + ( ( dx * ( i[ 1 ] - i[ 0 ] ) ) >> 8 );
		return a + ( ( dy * ( b - a) ) >> 8 );
	}

	void MarkerStripe::SampleFromImage(const cv::Mat& image)
	{
		if(Width <= 0 || Height <= 0 || Buffer != NULL)
			return;

		Buffer = new cv::Mat(Height, Width, CV_8UC1);
		unsigned char* data = Buffer->data;

		cv::Point2d it = Corners[0];

		for(int y = 0; y < Height; y++)
		{
			for(int x = 0; x < Width; x++, data++, it +=IterationNormalX)
			{
				*data = SampleSubPixelFromImage(image, it);
			}

			it = Corners[0] + IterationNormalY;
		}
	}

	void MarkerStripe::CalculateSubPixelCenter(void)
	{
		if(Buffer == NULL)
			return;

		unsigned char* row1Data = Buffer->data;
		unsigned char* row2Data = Buffer->data + Buffer->step;
		unsigned char* row3Data = Buffer->data + (2 * Buffer->step);
		int* derivative = new int[Width];

		for(int a = 1; a < Width - 1; a++) {
			derivative[a] = 
				(-1 * row1Data[a - 1]) + 
				(-2 * row2Data[a - 1]) + 
				(-1 * row3Data[a - 1]) +
				(1 * row1Data[a + 1]) + 
				(2 * row2Data[a + 1]) + 
				(1 * row3Data[a + 1]);
		}
		
		derivative[0] = derivative[1];
		derivative[Width - 1] = derivative[Width - 2];

		int min = 255;
		int minSum = 255 * 3;
		int minIndex = -1;
		
		// find discrete minimum
		for(int a = 1; a < Width - 1; a++)
		{
			int value = derivative[a];

			if(value <= min)
			{
				int sum = derivative[a - 1] + value + derivative[a + 1];

				if(sum < minSum)
				{
					min = value;
					minSum = sum;
					minIndex = a;
				}
			}
		}

		if(minIndex == -1)
			return;

		int left = derivative[minIndex - 1];
		int right = derivative[minIndex + 1];

		// y = ax²+bx+c
		// y'= 2ax+b 
		// c = min
		// b = (right - left) / 2
		// a = right - b

		double b = (right - left) / 2.0;
		double a = right - b;

		// xmin equals the offset to minIndex where the interpolated minimum is
		double xmin = a != 0 ? (-1.0 * b) / (2.0 * a) : 0;
		
		// calculate absolute sub pixel center from center
		SubPixelCenter = this->Center + (this->IterationNormalY * xmin);

		//std::cout << Center.x << " " << Center.y << std::endl;
		//std::cout << SubPixelCenter.x << " " << SubPixelCenter.y << std::endl << std::endl;

		delete derivative;
	}

	Marker::Marker(std::vector<cv::Point> corners) : Corners(corners) 
	{
	}

	Marker::Marker(const Marker& copy) : Corners(copy.Corners), Stripes(copy.Stripes)
	{
	}

	void NullImageProcessor::process(cv::Mat& input, cv::Mat& output)
	{
		output = input;
	}

	void ResizeImageProcessor::process(cv::Mat& input, cv::Mat& output)
	{
		cv::resize(input, output, cv::Size(), this->factor, this->factor);
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

				this->markers->push_back(marker);
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

		cv::polylines(image, &first, RectangleCorners, 1, true, cv::Scalar(0, 0, 255), 2);
		
		for(int a = 0; a < marker.Corners.size(); a++)
		{
			cv::circle(image, marker.Corners[a], 2, cv::Scalar(0, 255, 0), 2);
		}

		std::vector<cv::Point> stripeCorners;

		for(int a = 0; a < marker.Stripes.size(); a++)
		{
			stripeCorners.assign(marker.Stripes[a].Corners.begin(), marker.Stripes[a].Corners.end());
			const cv::Point* firstStripeCorner = &stripeCorners.front();

			cv::polylines(image, &firstStripeCorner, RectangleCorners, 1, true, cv::Scalar(255, 255, 0));
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