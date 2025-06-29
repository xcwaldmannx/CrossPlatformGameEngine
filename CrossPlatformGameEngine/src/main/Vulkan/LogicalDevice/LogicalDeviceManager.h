#pragma once

#include "../QueueFamily/QueueFamilyManager.h"

#include "../Surface/SurfaceManager.h"
#include "../PhysicalDevice/PhysicalDeviceManager.h"

#include <vulkan/vulkan.h>

namespace vulkan {

	class LogicalDeviceManager {
	public:
		VkResult create(const VkPhysicalDevice& physicalDevice, const QueueFamilyManager& queueFamilyManager);
		void destroy();

		const VkDevice& getDevice() const;
	private:
		VkDevice mVkDevice = nullptr;

		const std::vector<const char*> mDeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	};

}
