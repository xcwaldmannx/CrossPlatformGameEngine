#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace vulkan {

	class PhysicalDeviceManager {
	public:
		void establishPhysicalDevice(const VkInstance& instance,
			const VkSurfaceKHR& surface);

		const VkPhysicalDevice& getDevice() const;

	private:
		bool checkDeviceSupport(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);

	private:
		VkPhysicalDevice mVkPhysicalDevice = nullptr;

		const std::vector<const char*> mDeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	};

}
