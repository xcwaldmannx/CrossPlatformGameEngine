#pragma once

#include "Swapchain.h"
#include "../Types.h"

#include <cstdint>
#include <memory>

#include <vulkan/vulkan.h>

namespace ascen
{

	class SwapchainFactory
	{
	public:
		SwapchainFactory(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkSurfaceKHR surface);

		SwapchainPtr create(
			GLFWwindow* window,
			uint32_t graphicsFamily,
			uint32_t presentFamily) const;

	private:
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;
		VkSurfaceKHR mSurface = VK_NULL_HANDLE;
	};

}
