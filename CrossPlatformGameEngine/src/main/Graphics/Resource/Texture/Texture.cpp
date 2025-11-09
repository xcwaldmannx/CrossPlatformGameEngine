#include "Texture.h"

#include "../Image/Image.h"

#include <stdexcept>

using namespace ascen;

Texture Texture::create(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkQueue graphicsQueue,
	std::shared_ptr<CommandPool> commandPool,
	VkFormat format,
	VkImageTiling tiling,
	const std::vector<unsigned char>& pixels,
	uint32_t width,
	uint32_t height,
	uint32_t layers,
	VkImageUsageFlags usageFlags,
	VkMemoryPropertyFlags memoryFlags,
	VkImageAspectFlags aspectFlags)
{
	return Texture(
		physicalDevice,
		device, 
		graphicsQueue,
		commandPool,
		format,
		tiling,
		pixels,
		width,
		height,
		layers,
		usageFlags,
		memoryFlags,
		aspectFlags);
}

void Texture::destroy(VkDevice device, Texture& texture)
{
	Image::destroy(device, texture.mImage);
	vkDestroyImageView(device, texture.mView, nullptr);
}

VkImageView Texture::getView() const
{
	return mView;
}


Texture::Texture(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkQueue graphicsQueue,
	std::shared_ptr<CommandPool> commandPool,
	VkFormat format,
	VkImageTiling tiling,
	const std::vector<unsigned char>& pixels,
	uint32_t width,
	uint32_t height,
	uint32_t layers,
	VkImageUsageFlags usageFlags,
	VkMemoryPropertyFlags memoryFlags,
	VkImageAspectFlags aspectFlags)
{
	mImage = Image::create(
		physicalDevice, device,
		graphicsQueue, commandPool,
		format, tiling,
		pixels, width, height, layers,
		usageFlags, memoryFlags, aspectFlags);

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = mImage.mImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = layers;

	if (vkCreateImageView(
		device, &viewInfo, nullptr, &mView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}
}
