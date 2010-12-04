/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#include "Marker.h"

namespace TUMAugmentedRealityExercise
{
	
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
		
		// calculate absolute sub pixel center
		SubPixelCenter = this->Corners[0] + this->IterationNormalY + (this->IterationNormalX * (minIndex + xmin));

		delete derivative;
	}

	Marker::Marker(std::vector<cv::Point> corners) : Corners(corners) 
	{
		this->Pose = NULL;
	}

	Marker::Marker(const Marker& copy) : MarkerId(copy.MarkerId), Corners(copy.Corners), SubPixelCorners(copy.SubPixelCorners) , Stripes(copy.Stripes)
	{
		if(copy.Pose != NULL)
		{
			this->Pose = new float[16];
			std::copy(copy.Pose, copy.Pose + 16, this->Pose);
		}
	}

	Marker::~Marker(void)
	{
		if(this->Pose != NULL)
			delete this->Pose;
	}

	float Marker::RealSize = 0;

	void Marker::CalculateSubPixelCorners(void)
	{
		std::vector<cv::Point2f> points;
		cv::Vec4f firstLine(-1.0);
		cv::Vec4f lastLine(-1.0);
		cv::Vec4f line;

		for(int a = 0; a < this->Stripes.size(); a++)
		{
			points.push_back(this->Stripes[a].SubPixelCenter);

			if(points.size() == 6)
			{
				cv::fitLine(cv::Mat(points), line, CV_DIST_L2, 0, 0.01, 0.01);
				
				if(firstLine[0] < 0)
					firstLine = line;

				if(lastLine[0] > 0)
					this->SubPixelCorners.push_back(intersect(line, lastLine));

				lastLine = line;
				points.clear();
			}
		}
		
		this->SubPixelCorners.insert(this->SubPixelCorners.begin(), intersect(firstLine, lastLine));
	}

	bool Marker::SampleFromImageAndDecode(const cv::Mat& image)
	{
		cv::Mat buffer(6, 6, CV_8UC1);
		cv::Point2f points[4] = { cv::Point2f(-0.5, -0.5), cv::Point2f(5.5, -0.5), cv::Point2f(5.5, 5.5), cv::Point2f(-0.5, 5.5) };
		
		cv::Mat transform = cv::getPerspectiveTransform(&this->SubPixelCorners.front(), points);

		cv::warpPerspective(image, buffer, transform, cv::Size(6, 6));
		cv::threshold(buffer, buffer, 100, 255, CV_THRESH_BINARY);

		bool isMarkerBorderOk = cv::countNonZero(buffer.row(0)) + cv::countNonZero(buffer.row(5)) + cv::countNonZero(buffer.col(0)) + cv::countNonZero(buffer.col(5)) == 0;

		if(isMarkerBorderOk)
		{
			DebugImage::instance->Buffer = buffer.clone();
			
			unsigned char* data = buffer.data + buffer.step + 1;
			
			int codes[4] = { 0, 0, 0, 0 };

			for(int a = 0; a < 16; a++)
			{
				int col = a & 3;
				int row = a >> 2;

				int idx1 = row * 6 + col;
				int idx2 = 3 + (col * 6) - row;
				int idx3 = 21 - idx1;
				int idx4 = 21 - idx2;
				
				codes[0] <<= 1; 
				codes[1] <<= 1; 
				codes[2] <<= 1;
				codes[3] <<= 1;

				if(data[idx1] == 0)
					codes[0] |= 1;

				if(data[idx2] == 0)
					codes[1] |= 1;

				if(data[idx3] == 0)
					codes[2] |= 1;

				if(data[idx4] == 0)
					codes[3] |= 1;

				//std::cout << idx1 << "\t" << idx2 << "\t" << idx3  << "\t" << idx4 << std::endl;
			}

			int* minCodeIdx = std::min_element(codes, codes + 4);

			int code = *minCodeIdx;
			int rotation = minCodeIdx - codes;

			if(code == 0 || code == 0xffff)
				return false;

			if(rotation > 0) 
			{
				std::rotate(this->Corners.begin(), this->Corners.begin() + rotation, this->Corners.end());
				std::rotate(this->SubPixelCorners.begin(), this->SubPixelCorners.begin() + rotation, this->SubPixelCorners.end());
				std::rotate(this->Stripes.begin(), this->Stripes.begin() + rotation * 6, this->Stripes.end());
			}

			this->MarkerId = code;
		}

		return isMarkerBorderOk;
	}

	void Marker::EstimatePose(void)
	{
		if(this->Pose == NULL)
			this->Pose = new float[16];

		estimateSquarePose(this->Pose, (CvPoint2D32f*) &this->SubPixelCorners.front(), Marker::RealSize);
	}
}