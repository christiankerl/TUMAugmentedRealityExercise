/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#pragma once

#include <opencv\cv.h>
#include <opencv\highgui.h>

namespace TUMAugmentedRealityExercise
{
	class VideoSource
	{
	public:
		VideoSource(void) {};
		virtual ~VideoSource(void) {};

		virtual int GetFPS(void) = 0;

		virtual void GetNextImage(cv::Mat& buffer) = 0;
	};

	class CameraVideoSource : public VideoSource
	{
	private:
		cv::VideoCapture* video;
	public:
		CameraVideoSource(void);
		~CameraVideoSource(void);

		int GetFPS(void);

		void GetNextImage(cv::Mat& buffer);
	};

	
	class FileVideoSource : public VideoSource
	{
	private:
		CvCapture* capture;

		int GetFrames(void);
		int GetCurrentFrame(void);
		void SetCurrentFrame(int frame);
	public:
		FileVideoSource(const std::string& file);
		~FileVideoSource(void);

		int GetFPS(void);

		void GetNextImage(cv::Mat& buffer);
	};

	
	class StaticFileVideoSource : public VideoSource
	{
	private:
		cv::Mat buffer;
	public:
		StaticFileVideoSource(const std::string& file);
		~StaticFileVideoSource(void);

		int GetFPS(void);

		void GetNextImage(cv::Mat& buffer);
	};
}

