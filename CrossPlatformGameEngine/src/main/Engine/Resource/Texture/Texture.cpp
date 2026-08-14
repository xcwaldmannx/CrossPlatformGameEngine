#include "Texture.h"

#include <stdexcept>

using namespace ascen;

Texture::Texture(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	const CommandPoolPtr& commandPool,
	uint32_t width,
	uint32_t height,
	uint32_t layers,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usageFlags,
	VkMemoryPropertyFlags memoryFlags,
	VkImageAspectFlags aspectFlags) :
	mImage(
		physicalDevice, device, commandPool,
		width, height, layers,
		format, tiling, usageFlags, memoryFlags, aspectFlags)
{

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = mImage.handle();
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = layers;

	if (vkCreateImageView(
		device, &viewInfo, nullptr, &mHandle) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}
}

void Texture::update(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkQueue queue,
	const CommandPoolPtr& commandPool,
	const std::vector<unsigned char>& pixels)
{
	mImage.update(physicalDevice, device, queue, commandPool, pixels);
}

void Texture::destroy(VkDevice device)
{
	vkDestroyImageView(device, mHandle, nullptr);
	mImage.destroy(device);
}

Image& Texture::getImage()
{
	return mImage;
}
