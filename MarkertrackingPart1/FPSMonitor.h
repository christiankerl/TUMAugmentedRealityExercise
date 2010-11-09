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

namespace TUMAugmentedRealityExercise
{
	class FPSMonitor
	{
	private:
		int msPerFrame;
		double ticksPerMs;

		double processingStart;
		double processingEnd;
	public:
		FPSMonitor(void);
		~FPSMonitor(void);

		void SetVideoSourceFPS(int videoSourceFPS);

		void BeginProcessing(void);

		void EndProcessing(void);

		double GetElapsedMs(void);

		int GetMsUntilNextFrame(void);
	};
}

