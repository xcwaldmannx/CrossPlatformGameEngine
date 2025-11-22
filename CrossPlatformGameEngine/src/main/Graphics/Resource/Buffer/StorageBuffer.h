#pragma once

#include "Buffer2.h"

#include "../../Types.h"

#include <vector>

namespace ascen
{

	class StorageBuffer : public Buffer2
	{
	public:
		StorageBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			const CommandPoolPtr& commandPool,
			uint64_t itemCount,
			uint64_t itemSize);
	};

}
