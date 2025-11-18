#pragma once

#include "RenderPass.h"
#include "../Types.h"

#include <cstdint>
#include <memory>

#include <vulkan/vulkan.h>

namespace ascen
{

	class RenderPassFactory
	{
	public:
		RenderPassFactory(
			VkPhysicalDevice physicalDevice,
			VkDevice device);

		RenderPassPtr create(VkFormat colorFormat) const;

	private:
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
