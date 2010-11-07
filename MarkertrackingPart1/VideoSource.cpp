/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#include "VideoSource.h"

namespace TUMAugmentedRealityExercise
{
	CameraVideoSource::CameraVideoSource(void)
	{
		this->video = new cv::VideoCapture(0);
	}
	
	CameraVideoSource::~CameraVideoSource(void)
	{
		this->video->release();
	}

	void CameraVideoSource::GetNextImage(cv::Mat& buffer)
	{
		*this->video >> buffer;
	}
	
	FileVideoSource::FileVideoSource(const std::string& file)
	{
		this->capture = cvCreateFileCapture(file.c_str());
	}
	
	FileVideoSource::~FileVideoSource(void)
	{
		cvReleaseCapture(&this->capture);
	}

	void FileVideoSource::GetNextImage(cv::Mat& buffer)
	{
		if(this->GetFrames() <= this->GetCurrentFrame())
		{
			this->SetCurrentFrame(0);
		}
		
		buffer = cv::Mat(cvQueryFrame(this->capture));
	}

	int FileVideoSource::GetFrames(void)
	{
		return (int) cvGetCaptureProperty(this->capture, CV_CAP_PROP_FRAME_COUNT);
	}

	int FileVideoSource::GetCurrentFrame(void)
	{
		return (int) cvGetCaptureProperty(this->capture, CV_CAP_PROP_POS_FRAMES);
	}

	void FileVideoSource::SetCurrentFrame(int frame)
	{
		cvSetCaptureProperty(this->capture, CV_CAP_PROP_POS_FRAMES, frame);
	}
}