#pragma once

#include "../VulkanInstance/VulkanInstance.h"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <vulkan/vulkan.h>

namespace ascen {

	struct Surface {
		VkSurfaceKHR mSurface;
	};

	/*
	FILE: SurfaceHandler.h
	NAME: createSurface
	RTRN: void
	DESC: Handles Surface creation.
	*/
	void createSurface(Instance& instance, GLFWwindow* window, Surface* sSurface);

	/*
	FILE: SurfaceHandler.h
	NAME: destroySurface
	RTRN: void
	DESC: Handles Surface destruction.
	*/
	void destroySurface(Instance& instance, Surface& surface);

};