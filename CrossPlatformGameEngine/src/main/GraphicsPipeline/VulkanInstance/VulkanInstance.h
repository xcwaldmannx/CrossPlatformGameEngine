#pragma once

#include "../ValidationLayers/ValidationLayers.h"
#include "../Extensions/Extensions.h"
#include "../DebugMessenger/DebugMessenger.h"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <vulkan/vulkan.h>

/*
FILE: VulkanInstance.h
NAME: VulkanInstance
TYPE: CLASS
DESC: Vulkan Instance creation
*/

namespace ascen {

	struct Instance {
		VkInstance mInstance = nullptr;
		ValidationLayers mValidationLayers;
		Extensions mExtensions;
		DebugMessenger mDebugMessenger;
	};

	void createVulkanInstance(Instance* instance);
	void destroyVulkanInstance(Instance& instance);

}
