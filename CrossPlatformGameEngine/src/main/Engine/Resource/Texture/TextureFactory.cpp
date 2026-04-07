#include "TextureFactory.h"

#include "Image/ImageTexture.h"
#include "Depth/DepthTexture.h"

using namespace ascen;

TextureFactory::TextureFactory(
	VkPhysicalDevice physicalDevice,
	VkDevice device) : mPhysicalDevice(physicalDevice), mDevice(device) {
}

TexturePtr TextureFactory::createImage(
	const CommandPoolPtr& commandPool,
	uint32_t width,
	uint32_t height,
	uint32_t layers) const
{
	return std::make_shared<ImageTexture>(
		mPhysicalDevice,
		mDevice,
		commandPool,
		width,
		height,
		layers);
}

TexturePtr TextureFactory::createDepth(
	const CommandPoolPtr& commandPool,
	uint32_t width,
	uint32_t height) const
{
	return std::make_shared<DepthTexture>(
		mPhysicalDevice,
		mDevice,
		commandPool,
		width,
		height);
}
