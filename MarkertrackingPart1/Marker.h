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

#include "VectorUtil.h"
#include "PoseEstimation.h"

#include "DebugImage.h"

namespace TUMAugmentedRealityExercise
{
	class MarkerStripe
	{
	private:
		int SampleSubPixelFromImage(const cv::Mat image, cv::Point2d p);
	public:
		cv::Mat* Buffer;

		cv::Point2d Center;
		cv::Point2d SubPixelCenter;
		std::vector<cv::Point2d> Corners;

		int Width;
		int Height;

		cv::Point2d IterationNormalX;
		cv::Point2d IterationNormalY;

		MarkerStripe(void);
		MarkerStripe(const MarkerStripe& copy);
		~MarkerStripe(void);

		void SampleFromImage(const cv::Mat& image);
		void CalculateSubPixelCenter(void);
	};

	class Marker
	{
	public:
		static float RealSize;

		int MarkerId;
		float* Pose;

		std::vector<cv::Point> Corners;
		std::vector<cv::Point2f> SubPixelCorners;
		std::vector<MarkerStripe> Stripes;

		Marker(std::vector<cv::Point> corners);
		Marker(const Marker& copy);
		~Marker(void);

		void CalculateSubPixelCorners(void);

		bool SampleFromImageAndDecode(const cv::Mat& image);

		void EstimatePose(void);
	};
	
	typedef std::vector<Marker> MarkerContainer;
}