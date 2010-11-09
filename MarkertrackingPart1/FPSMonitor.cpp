/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#include "FPSMonitor.h"

namespace TUMAugmentedRealityExercise
{
	FPSMonitor::FPSMonitor(void)
	{
		this->ticksPerMs = cv::getTickFrequency() / 1000;
	}

	FPSMonitor::~FPSMonitor(void)
	{
	}

	void FPSMonitor::SetVideoSourceFPS(int videoSourceFPS)
	{
		if(videoSourceFPS == 0)
			videoSourceFPS = 30;

		this->msPerFrame = 1000.0 / videoSourceFPS;
	}

	void FPSMonitor::BeginProcessing(void)
	{
		this->processingStart = cv::getTickCount();
	}

	void FPSMonitor::EndProcessing(void)
	{
		this->processingEnd = cv::getTickCount();
	}

	double FPSMonitor::GetElapsedMs(void)
	{
		return (this->processingEnd - this->processingStart) / this->ticksPerMs;
	}

	int FPSMonitor::GetMsUntilNextFrame(void)
	{
		return std::max(1, (int)(this->msPerFrame - this->GetElapsedMs()));
	}
}
