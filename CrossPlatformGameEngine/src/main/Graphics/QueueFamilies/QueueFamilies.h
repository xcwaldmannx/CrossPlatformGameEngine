#pragma once

#include <stdint>

#include <optional>

#include <vulkan/vulkan.h>

namespace ascen
{
	using QueueFamily = std::optional<uint32_t>;

	class QueueFamilies
	{
	public:
		static void updateQueueFamilies(
			VkPhysicalDevice physicalDevice,
			VkSurfaceKHR surface,
			QueueFamily& graphicsFamily,
			QueueFamily& presentFamily);

		static VkQueue getDeviceQueue(VkDevice device, uint32_t index);
	};

}
