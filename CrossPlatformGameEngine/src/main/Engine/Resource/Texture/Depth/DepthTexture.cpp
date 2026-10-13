#include "DepthTexture.h"

#include "../../../Device/Physical/PhysicalDevice.h"

using namespace ascen;

DepthTexture::DepthTexture(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	const CommandPoolPtr& commandPool,
	dim::Extent2D extent) :
	Texture(
		physicalDevice,
		device,
		commandPool,
		extent,
		1,
		PhysicalDevice::findDepthFormat(physicalDevice),
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_IMAGE_ASPECT_DEPTH_BIT) {}