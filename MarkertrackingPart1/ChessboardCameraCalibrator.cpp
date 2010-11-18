/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#include "ChessboardCameraCalibrator.h"

namespace TUMAugmentedRealityExercise
{
	ChessboardCameraCalibrator::ChessboardCameraCalibrator(const std::string& outputFolder) :
		patternSize(8, 6), 
		sampledCorners(), 
		intrinsicParameters(cv::Mat::zeros(3, 3, CV_64FC1)),
		distortionParameters(cv::Mat::zeros(1, 5, CV_64FC1)),
		outputFolder(outputFolder),
		sampledObjectPrototype()
	{
		for(int i = 0; i < patternSize.area(); i++)
		{
			this->sampledObjectPrototype.push_back(cv::Point3f(i / patternSize.width, i % patternSize.width, 0.0));
		}
	}

	ChessboardCameraCalibrator::~ChessboardCameraCalibrator(void)
	{
	}

	int ChessboardCameraCalibrator::GetSampledImageCount(void)
	{
		return this->sampledCorners.size();
	}

	std::string ChessboardCameraCalibrator::GetImageOutputFolder(void)
	{
		std::string result(this->outputFolder);
		return result.append("/img");
	}

	std::string ChessboardCameraCalibrator::GetMarkedImageOutputFolder(void)
	{
		return GetImageOutputFolder().append("/marked");
	}

	void ChessboardCameraCalibrator::SaveImage(const cv::Mat& image, bool marked)
	{
		std::string file;
		int n = GetSampledImageCount();		
		std::string prefix = (n < 10 ? "_0" : "_");

		file += marked ? GetMarkedImageOutputFolder() : GetImageOutputFolder();
		file += "/chessboard" + prefix + std::to_string((long double) n) + ".bmp";

		cvSaveImage(file.c_str(), &(CvMat) image);
	}

	bool ChessboardCameraCalibrator::SampleChessboardImage(const cv::Mat& image)
	{
		if(this->imageSize.area() == 0)
		{
			this->imageSize = image.size();
		}
		else if(this->imageSize.area() != image.size().area())
		{
			return false;
		}

		cv::Mat grey;
		
		if(image.channels() == 3)
		{
			cv::cvtColor(image, grey, CV_BGR2GRAY);
		}
		else
		{
			grey = image;
		}

		std::vector<cv::Point2f > corners(this->patternSize.area());

		bool success = cvFindChessboardCorners(&(CvMat) grey, this->patternSize, (CvPoint2D32f*)&corners.front());

		if(success)
		{
			cvFindCornerSubPix(&(CvMat) grey, (CvPoint2D32f*)&corners.front(), corners.size(), cvSize(11, 11), cvSize(-1, -1), cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1)); 
			
			SaveImage(grey, false);
			cvDrawChessboardCorners(&(CvMat) grey, this->patternSize, (CvPoint2D32f*)&corners.front(), corners.size(), 1);
			SaveImage(grey, true);

			this->sampledCorners.push_back(corners);
		}

		return success;
	}

	void ChessboardCameraCalibrator::SampleChessboardImages(const std::vector<std::string> files)
	{
		for(int a = 0; a < files.size(); a++)
		{
			SampleChessboardImage(cv::Mat(cvLoadImageM(files[a].c_str(), 0)));
		}
	}

	void ChessboardCameraCalibrator::CalculateCalibration(void)
	{
		cv::Mat objectPoints(this->sampledCorners.size() * this->sampledObjectPrototype.size(), 3, CV_32FC1);
		cv::Mat imagePoints(this->sampledCorners.size() * this->sampledObjectPrototype.size(), 2, CV_32FC1);
		cv::Mat pointCounts(this->sampledCorners.size(), 1, CV_32SC1);

		cv::Mat rotations(this->sampledCorners.size(), 3, CV_32FC1);
		cv::Mat translations(this->sampledCorners.size(), 3, CV_32FC1);

		for(int a = 0; a < this->sampledCorners.size(); a++)
		{
			for(int b = 0; b < this->sampledObjectPrototype.size(); b++)
			{
				int offset =  a * this->sampledObjectPrototype.size();

				objectPoints.at<float>(offset + b, 0) = this->sampledObjectPrototype[b].x;
				objectPoints.at<float>(offset + b, 1) = this->sampledObjectPrototype[b].y;
				objectPoints.at<float>(offset + b, 2) = 0.0f;

				imagePoints.at<float>(offset + b, 0) = this->sampledCorners[a][b].x;
				imagePoints.at<float>(offset + b, 1) = this->sampledCorners[a][b].y;
			}

			pointCounts.at<int>(a, 0) = this->sampledObjectPrototype.size();
		}
		
		try
		{
			double error = cvCalibrateCamera2(
				&(CvMat) objectPoints, 
				&(CvMat) imagePoints,
				&(CvMat) pointCounts, 
				this->imageSize, 
				&(CvMat) this->intrinsicParameters, 
				&(CvMat) this->distortionParameters,
				&(CvMat) rotations,
				&(CvMat) translations
			);
			
			std::cout << "[" << intrinsicParameters.at<double>(0, 0) << ", " << intrinsicParameters.at<double>(0, 1) << ", " << intrinsicParameters.at<double>(0, 2) << "]" << std::endl;
			std::cout << "[" << intrinsicParameters.at<double>(1, 0) << ", " << intrinsicParameters.at<double>(1, 1) << ", " << intrinsicParameters.at<double>(1, 2) << "]" << std::endl;
			std::cout << "[" << intrinsicParameters.at<double>(2, 0) << ", " << intrinsicParameters.at<double>(2, 1) << ", " << intrinsicParameters.at<double>(2, 2) << "]" << std::endl;
			std::cout << std::endl;
			std::cout << "[" << distortionParameters.at<double>(0, 0) << ", " << distortionParameters.at<double>(0, 1) << ", " << distortionParameters.at<double>(0, 2) << ", " << distortionParameters.at<double>(0, 3) << ", " << distortionParameters.at<double>(0, 4) << "]" << std::endl;
			
			std::cout << error << std::endl;			

			CvFileStorage* storage = cvOpenFileStorage((this->outputFolder + "/camera.xml").c_str(), 0, CV_STORAGE_WRITE);

			cvWrite(storage, "intrinsic", &(CvMat) this->intrinsicParameters);
			cvWrite(storage, "distortion", &(CvMat) this->distortionParameters);

			cvReleaseFileStorage(&storage);
		}
		catch(cv::Exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
}
