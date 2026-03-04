#pragma once

#include "../Buffer.h"
#include "../../../Core/Types.h"

namespace ascen
{

	class StorageBuffer : public Buffer
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
