#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <vulkan/vulkan.h>

namespace vulkan {

	class SurfaceManager {
	public:
		SurfaceManager();

		VkResult create(const VkInstance& instance, GLFWwindow* window);
		void destroy(const VkInstance& instance);
		
		const VkSurfaceKHR& getSurface() const;

	private:
		VkSurfaceKHR mVkSurface = nullptr;
	};

}
