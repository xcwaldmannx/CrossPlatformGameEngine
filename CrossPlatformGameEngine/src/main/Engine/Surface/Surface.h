#pragma once


#include <vulkan/vulkan.h>

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

namespace ascen
{
	class Surface
	{
	public:
		static VkSurfaceKHR create(VkInstance instance, GLFWwindow* window);
		static void destroy(VkInstance instance, VkSurfaceKHR surface);
	};
}