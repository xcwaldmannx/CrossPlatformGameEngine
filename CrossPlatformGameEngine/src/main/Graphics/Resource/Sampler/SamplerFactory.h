#pragma once

#include "../../Types.h"
#include "Sampler2.h"

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
