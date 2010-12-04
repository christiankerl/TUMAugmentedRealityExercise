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

namespace TUMAugmentedRealityExercise
{
	class DebugImage
	{
	public:
		static DebugImage* instance;
		
		cv::Mat Buffer;

		DebugImage(void) { instance = this; };
		~DebugImage(void) { instance = NULL; };
	};
}

