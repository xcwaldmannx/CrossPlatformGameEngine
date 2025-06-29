#pragma once

#include <vulkan/vulkan.h>

#include "../Buffer/Buffer.h"

namespace vulkan {

	class MemoryAllocator {
	public:
		MemoryAllocator();
		MemoryAllocator(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice);
		void init(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice);

		void allocate(Buffer& buffer, VkMemoryPropertyFlags properties);
	private:
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	private:
		VkPhysicalDevice mPhysicalDevice = nullptr;
		VkDevice mLogicalDevice = nullptr;
	};

}