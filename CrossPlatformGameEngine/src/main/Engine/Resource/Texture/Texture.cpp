#include "Texture.h"

#include <stdexcept>
#include <cmath>

using namespace ascen;

Texture::Texture(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	const CommandPoolPtr& commandPool,
	dim::Extent2D extent,
	bool generateMips,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usageFlags,
	VkMemoryPropertyFlags memoryFlags,
	VkImageAspectFlags aspectFlags) :
	mImage(
		physicalDevice, device, commandPool,
		extent, generateMips, format, tiling, usageFlags, memoryFlags, aspectFlags)
{

	if (extent.mMode == dim::Extent2D::Mode::FIXED && (extent.mExtent.width == 0 || extent.mExtent.height == 0))
	{
		throw std::runtime_error("Attempting to create an image with zero dimensions");
	}

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = mImage.handle();
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	if (generateMips)
	{
		viewInfo.subresourceRange.levelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(extent.mExtent.width, extent.mExtent.height))) + 1);
	}
	else
	{
		viewInfo.subresourceRange.levelCount = 1;
	}
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

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
