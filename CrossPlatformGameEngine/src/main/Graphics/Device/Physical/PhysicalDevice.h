#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{
	static const std::vector<const char*> mDeviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	class PhysicalDevice
	{
	public:
		static VkPhysicalDevice get(VkInstance instance, VkSurfaceKHR surface);
		static void destroy();

		static VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);

	private:
		static bool checkDeviceSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	};

}
