#pragma once

#include "../PhysicalDeviceHandler/PhysicalDeviceHandler.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen {

	struct LogicalDevice {
		VkDevice mDevice;
	};

	/*
	FILE: LogicalDeviceHandler.h
	NAME: createLogicalDevice
	RTRN: void
	DESC: Handles Logical Device creation.
	*/
	void createLogicalDevice(
		PhysicalDevice& physicalDevice,
		uint32_t graphicsFamily,
		uint32_t presentFamily,
		LogicalDevice* logicalDevice);

	/*
	FILE: LogicalDeviceHandler.h
	NAME: destroyLogicalDevice
	RTRN: void
	DESC: Handles Logical Device destruction.
	*/
	void destroyLogicalDevice(LogicalDevice& logicalDevice);

}