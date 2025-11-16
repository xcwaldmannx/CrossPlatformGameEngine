#include "ImageTexture.h"

#include "../../QueueFamilies/QueueFamilies.h"

using namespace ascen;

ImageTexture::ImageTexture(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	uint32_t queueFamilyIndex,
	const CommandPoolPtr& commandPool,
	const std::vector<unsigned char>& pixels,
	uint32_t width,
	uint32_t height,
	uint32_t layers) :
	Texture2(
		physicalDevice,
		device,
		commandPool,
		width,
		height,
		layers,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT)
{
	VkQueue queue = QueueFamilies::getDeviceQueue(device, queueFamilyIndex);
	mImage.update(
		physicalDevice, device, queue, commandPool,
		pixels, width, height, layers,
		VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}