/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#include "MemoryStorage.h"

namespace TUMAugmentedRealityExercise
{
	MemoryStorage::MemoryStorage(void)
	{
		this->memory = cvCreateMemStorage();
	}

	MemoryStorage::~MemoryStorage(void)
	{
		cvReleaseMemStorage(&this->memory);
	}

	CvMemStorage* MemoryStorage::GetPointer(void) const
	{
		return this->memory;
	}

	void MemoryStorage::Clear(void)
	{
		cvClearMemStorage(this->memory);
	}
}
