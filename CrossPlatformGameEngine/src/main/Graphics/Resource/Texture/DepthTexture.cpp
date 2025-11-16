#include "DepthTexture.h"

#include "../../Device/Physical/PhysicalDevice.h"
#include "../../QueueFamilies/QueueFamilies.h"

using namespace ascen;

DepthTexture::DepthTexture(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	uint32_t queueFamilyIndex,
	const CommandPoolPtr& commandPool,
	uint32_t width,
	uint32_t height) :
	Texture2(
		physicalDevice,
		device,
		commandPool,
		width,
		height,
		1,
		PhysicalDevice::findDepthFormat(physicalDevice),
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_IMAGE_ASPECT_DEPTH_BIT) {}