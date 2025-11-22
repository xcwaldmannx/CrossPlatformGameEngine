#include "Image2.h"

#include "../Barrier/Barrier.h"

#include <stdexcept>

using namespace ascen;

Image2::Image2(
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
	mWidth(width),
	mHeight(height),
	mLayers(layers),
	mFormat(format),
	mAspectFlags(aspectFlags)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = mWidth;
	imageInfo.extent.height = mHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = mLayers;
	imageInfo.format = mFormat;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usageFlags;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(device, &imageInfo, nullptr, &mHandle) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	Memory mem = getMemoryInfo(physicalDevice, device, mHandle, memoryFlags);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = mem.mSize;
	allocInfo.memoryTypeIndex = mem.mTypeIndex;

	if (vkAllocateMemory(device, &allocInfo, nullptr, &mMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device, mHandle, mMemory, 0);
}

void Image2::update(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkQueue queue,
	const CommandPoolPtr& commandPool,
	const std::vector<unsigned char>& pixels)
{
	VkDeviceSize pixelCount = static_cast<VkDeviceSize>(mWidth * mHeight * mLayers);
	size_t pixelSize = 4;
	VkDeviceSize imageSizeBytes = static_cast<VkDeviceSize>(pixelCount * pixelSize);

	Buffer2 stagingBuffer(
		physicalDevice,
		device,
		pixelCount,
		pixelSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data = nullptr;
	vkMapMemory(device, stagingBuffer.getMemory(), 0, imageSizeBytes, 0, &data);
	memcpy(data, pixels.data(), static_cast<size_t>(imageSizeBytes));
	vkUnmapMemory(device, stagingBuffer.getMemory());

	transitionLayout(
		device,
		queue,
		commandPool,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	copy(device, queue, commandPool, stagingBuffer, *this);

	transitionLayout(
		device,
		queue,
		commandPool,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	stagingBuffer.destroy(device);
}

void Image2::create(VkDevice device)
{
	// not used, remove later
}

void Image2::destroy(VkDevice device)
{
	vkDestroyImage(device, mHandle, nullptr);
	vkFreeMemory(device, mMemory, nullptr);
}

void Image2::transitionLayout(
	VkDevice device,
	VkQueue queue,
	const CommandPoolPtr& commandPool,
	VkImageLayout oldLayout,
	VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = commandPool->beginSingle(device);

	VkAccessFlags2 srcAccess;
	VkPipelineStageFlags2 srcStage;

	VkAccessFlags2 dstAccess;
	VkPipelineStageFlags2 dstStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		srcAccess = VK_ACCESS_2_NONE;
		srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		dstAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		dstStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		srcStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		dstAccess = VK_ACCESS_2_SHADER_READ_BIT;
		dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		srcAccess = VK_ACCESS_2_NONE;
		srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		dstAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dstStage = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	Barrier::image(
		commandBuffer, mHandle, mLayers,
		oldLayout, newLayout,
		srcAccess, srcStage,
		dstAccess, dstStage,
		mAspectFlags);

	commandPool->endSingle(device, queue, commandBuffer);
}

void Image2::copy(
	VkDevice device,
	VkQueue queue,
	const CommandPoolPtr& commandPool,
	Buffer2& buffer,
	Image2& image)
{
	VkCommandBuffer commandBuffer = commandPool->beginSingle(device);

	VkDeviceSize layerSize = static_cast<VkDeviceSize>(mWidth * mHeight * 4);

	std::vector<VkBufferImageCopy> regions(mLayers);

	for (uint32_t layer = 0; layer < mLayers; layer++)
	{
		VkBufferImageCopy& region = regions[layer];
		region.bufferOffset = layer * layerSize;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = mAspectFlags;

		if (mFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || mFormat == VK_FORMAT_D24_UNORM_S8_UINT)
		{
			region.imageSubresource.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = layer;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent =
		{
			mWidth,
			mHeight,
			1
		};
	}

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer.handle(),
		image.mHandle,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		static_cast<uint32_t>(regions.size()),
		regions.data());

	commandPool->endSingle(device, queue, commandBuffer);
}

Image2::Memory Image2::getMemoryInfo(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkImage image,
	VkMemoryPropertyFlags memoryFlags)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	Memory mem{};

	mem.mSize = memRequirements.size;

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & memoryFlags) == memoryFlags)
		{
			mem.mTypeIndex = i;
			break;
		}
	}

	return mem;
}
