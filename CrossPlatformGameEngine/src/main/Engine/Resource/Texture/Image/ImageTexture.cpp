#include "ImageTexture.h"

using namespace ascen;

ImageTexture::ImageTexture(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	const CommandPoolPtr& commandPool,
	dim::Extent2D extent,
	uint32_t layers) :
	Texture(
		physicalDevice,
		device,
		commandPool,
		extent,
		layers,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT) {}