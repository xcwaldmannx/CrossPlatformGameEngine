#include "TextureFactory.h"

using namespace ascen;

TextureFactory::TextureFactory(
	VkPhysicalDevice physicalDevice,
	VkDevice device) : mPhysicalDevice(physicalDevice), mDevice(device) {
}

TexturePtr TextureFactory::createImage(
	uint32_t queueFamilyIndex,
	const CommandPoolPtr& commandPool,
	const std::vector<unsigned char>& pixels,
	uint32_t width,
	uint32_t height,
	uint32_t layers) const
{
	return std::make_shared<ImageTexture>(
		mPhysicalDevice,
		mDevice,
		queueFamilyIndex,
		commandPool,
		pixels,
		width,
		height,
		layers);
}

TexturePtr TextureFactory::createDepth(
	uint32_t queueFamilyIndex,
	const CommandPoolPtr& commandPool,
	uint32_t width,
	uint32_t height) const
{
	return std::make_shared<DepthTexture>(
		mPhysicalDevice,
		mDevice,
		queueFamilyIndex,
		commandPool,
		width,
		height);
}
