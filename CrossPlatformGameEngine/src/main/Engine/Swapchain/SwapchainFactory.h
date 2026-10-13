#pragma once

#include "../Core/Types.h"

#include <cstdint>
#include <memory>

#include <GLFW/glfw3.h>
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
