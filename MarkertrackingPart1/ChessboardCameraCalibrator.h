/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#pragma once

#include <iostream>

#include <opencv\cv.h>
#include <opencv\highgui.h>

namespace TUMAugmentedRealityExercise
{
	class ChessboardCameraCalibrator
	{
	private:
		cv::Size patternSize;
		cv::Size imageSize;

		const std::string outputFolder;

		cv::Mat intrinsicParameters;
		cv::Mat distortionParameters;

		std::vector<cv::Point3f> sampledObjectPrototype;
		std::vector<std::vector<cv::Point2f> > sampledCorners;

		std::string GetImageOutputFolder(void);
		std::string GetMarkedImageOutputFolder(void);

		void SaveImage(const cv::Mat& image, bool marked);
	public:
		ChessboardCameraCalibrator(const std::string& outputFolder);
		~ChessboardCameraCalibrator(void);

		int GetSampledImageCount(void);

		bool SampleChessboardImage(const cv::Mat& image);

		void SampleChessboardImages(const std::vector<std::string> files);

		void CalculateCalibration(void);
	};
}

