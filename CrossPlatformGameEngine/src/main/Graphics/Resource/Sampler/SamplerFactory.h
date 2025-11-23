#pragma once

#include "../../Core/Types.h"

#include <vulkan/vulkan.h>

namespace ascen
{

	class SamplerFactory
	{
	public:
		SamplerFactory(VkPhysicalDevice physicalDevice, VkDevice device);

		SamplerPtr createSampler() const;

	private:
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
