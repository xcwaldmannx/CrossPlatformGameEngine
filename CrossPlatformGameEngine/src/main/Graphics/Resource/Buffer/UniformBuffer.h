#pragma once

#include "Buffer2.h"

#include "../../Types.h"

#include <vector>

namespace ascen
{

	template<typename T>
	class UniformBuffer : public Buffer2
	{
	public:
		UniformBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint64_t size) :
			Buffer2(
				physicalDevice,
				device,
				size,
				sizeof(T),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
		{
			VkDeviceSize sizeBytes = size * sizeof(T);
			vkMapMemory(device, mMemory, 0, sizeBytes, 0, &mMappedMemory);
		}
	};

}
