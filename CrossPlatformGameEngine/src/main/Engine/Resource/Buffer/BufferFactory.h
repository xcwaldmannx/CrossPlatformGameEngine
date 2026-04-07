#pragma once

#include "../../Core/Types.h"

#include "../../Registry/Resource/ResourceRegistry.h"

#include <vulkan/vulkan.h>

namespace ascen
{

	class BufferFactory
	{
	public:
		BufferFactory(
			VkPhysicalDevice physicalDevice,
			VkDevice device);

		BufferPtr create(
			uint32_t itemCount,
			uint32_t itemSize,
			BufferUsageFlags usageFlags,
			BufferMemoryFlags memoryFlags) const;

	private:
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
