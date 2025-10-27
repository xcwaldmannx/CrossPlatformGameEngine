#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{
	static const std::vector<const char*> sDeviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	class Device
	{
	public:
		static VkDevice create(
			VkPhysicalDevice physicalDevice,
			uint32_t graphicsFamily,
			uint32_t presentFamily);

		static void destroy(VkDevice device);
	};

}
