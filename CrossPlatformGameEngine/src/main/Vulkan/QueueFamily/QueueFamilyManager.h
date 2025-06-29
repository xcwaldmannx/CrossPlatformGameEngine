#pragma once

#include "../Surface/SurfaceManager.h"
#include "../PhysicalDevice/PhysicalDeviceManager.h"

#include <optional>

#include <vulkan/vulkan.h>

namespace vulkan {

	class QueueFamilyManager {
	public:
		void init(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
		void updateQueueFamilies();

		uint32_t getGraphicsFamilyIndex() const;
		uint32_t getPresentFamilyIndex() const;

		VkQueue getDeviceQueue(const VkDevice& logicalDevice, uint32_t index);

	private:
		bool mIsInitialized = false;

		VkPhysicalDevice mPhysicalDevice = nullptr;
		VkSurfaceKHR mSurface = nullptr;

		std::optional<uint32_t> mGraphicsFamily;
		std::optional<uint32_t> mPresentFamily;
	};

}
