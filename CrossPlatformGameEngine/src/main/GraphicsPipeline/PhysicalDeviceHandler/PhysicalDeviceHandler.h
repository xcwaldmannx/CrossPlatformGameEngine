#pragma once

#include "../SurfaceHandler/SurfaceHandler.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen {

	struct PhysicalDevice {
		VkPhysicalDevice mDevice;
	};
	
	void pickPhysicalDevice(Instance& instance, Surface& surface, PhysicalDevice* physicalDevice);

}