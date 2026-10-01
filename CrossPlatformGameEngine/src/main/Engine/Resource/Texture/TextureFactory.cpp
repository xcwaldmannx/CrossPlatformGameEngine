#include "TextureFactory.h"

#include "Image/ImageTexture.h"
#include "Writable/WritableTexture.h"
#include "Depth/DepthTexture.h"

using namespace ascen;

TextureFactory::TextureFactory(
	VkPhysicalDevice physicalDevice,
	VkDevice device) : mPhysicalDevice(physicalDevice), mDevice(device) {
}

TexturePtr TextureFactory::createImage(
	const CommandPoolPtr& commandPool,
	dim::Extent2D extent,
	uint32_t layers) const
{
	return std::make_shared<ImageTexture>(
		mPhysicalDevice,
		mDevice,
		commandPool,
		extent,
		layers);
}

TexturePtr TextureFactory::createWritable(
	const CommandPoolPtr& commandPool,
	const Format format,
	dim::Extent2D extent,
	uint32_t layers) const
{
	return std::make_shared<WritableTexture>(
		mPhysicalDevice,
		mDevice,
		commandPool,
		format,
		extent,
		layers);
}

TexturePtr TextureFactory::createDepth(
	const CommandPoolPtr& commandPool,
	dim::Extent2D extent) const
{
	return std::make_shared<DepthTexture>(
		mPhysicalDevice,
		mDevice,
		commandPool,
		extent);
}
