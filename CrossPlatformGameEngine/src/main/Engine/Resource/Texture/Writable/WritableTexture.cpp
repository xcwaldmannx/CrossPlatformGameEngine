#include "WritableTexture.h"

using namespace ascen;

WritableTexture::WritableTexture(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	const CommandPoolPtr& commandPool,
	const Format format,
	uint32_t width,
	uint32_t height,
	uint32_t layers) :
	Texture(
		physicalDevice,
		device,
		commandPool,
		width,
		height,
		layers,
		static_cast<VkFormat>(format),
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT) {}