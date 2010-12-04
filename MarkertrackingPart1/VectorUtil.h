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
	double length(cv::Point2d vector);

	cv::Point2d normalize(cv::Point2d vector);

	cv::Point2f intersect(cv::Vec4f a, cv::Vec4f b);
}