#pragma once

#include "../SurfaceHandler/SurfaceHandler.h"
#include "../PhysicalDeviceHandler/PhysicalDeviceHandler.h"
#include "../LogicalDeviceHandler/LogicalDeviceHandler.h"

#include <optional>

#include <vulkan/vulkan.h>

namespace ascen {

	/*
	FILE: QueueFamilyHandler.h
	NAME: updateQueueFamilies
	RTRN: void
	DESC: Updates suitable Queue Families for graphics and present queues.
	*/
	void updateQueueFamilies(
		PhysicalDevice& physicalDevice,
		Surface& surface,
		std::optional<uint32_t>& graphicsFamily,
		std::optional<uint32_t>& presentFamily);

	VkQueue getDeviceQueue(LogicalDevice& logicalDevice, uint32_t index);

}