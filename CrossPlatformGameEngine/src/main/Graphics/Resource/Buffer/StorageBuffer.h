#pragma once

#include "Buffer2.h"

#include "../../Types.h"

#include <vector>

namespace ascen
{

	template<typename T>
	class StorageBuffer : public Buffer2
	{
	public:
		StorageBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<T>& storage) :
			Buffer2(
				physicalDevice,
				device,
				storage.size(),
				sizeof(T),
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
		{
			update<T>(physicalDevice, device, queueFamilyIndex, commandPool, storage);
		}
	};

}
