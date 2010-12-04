/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#include "VectorUtil.h"

namespace TUMAugmentedRealityExercise
{
	double length(cv::Point2d vector)
	{
		return sqrt(vector.x*vector.x + vector.y*vector.y);
	}

	cv::Point2d normalize(cv::Point2d vector)
	{
		double norm =  1.0 / length(vector);

		return cv::Point2d(norm * vector.x, norm * vector.y);
	}

	cv::Point2f intersect(cv::Vec4f a, cv::Vec4f b)
	{
		cv::Mat left(2, 2, CV_32FC1);
		left.at<float>(0, 0) = a[0];
		left.at<float>(0, 1) = -b[0];
		left.at<float>(1, 0) = a[1];
		left.at<float>(1, 1) = -b[1];

		cv::Mat right(2, 1, CV_32FC1);
		right.at<float>(0, 0) = b[2] - a[2];
		right.at<float>(1, 0) = b[3] - a[3];

		cv::Mat result(2, 1, CV_32FC1);

		cv::solve(left, right, result);

		return cv::Point2f(result.at<float>(0, 0) * a[0] + a[2], result.at<float>(0, 0) * a[1] + a[3]);
	}
}