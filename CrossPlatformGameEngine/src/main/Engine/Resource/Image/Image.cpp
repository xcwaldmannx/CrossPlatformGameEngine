#include "Image.h"

#include "../Barrier/Barrier.h"

#include <stdexcept>
#include <string.h>
#include <cmath>

using namespace ascen;

Image::Image(
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
	mExtent(extent),
	mFormat(format),
	mAspectFlags(aspectFlags)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = extent.mExtent.width;
	imageInfo.extent.height = extent.mExtent.height;
	imageInfo.extent.depth = 1;
	if (generateMips)
	{
		mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(extent.mExtent.width, extent.mExtent.height))) + 1);
		imageInfo.mipLevels = mMipLevels;
	}
	else
	{
		mMipLevels = 1;
		imageInfo.mipLevels = mMipLevels;
	}
	imageInfo.arrayLayers = 1;
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

void Image::update(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkQueue queue,
	const CommandPoolPtr& commandPool,
	const std::vector<unsigned char>& pixels)
{
	uint32_t imageSizeBytes = static_cast<uint32_t>(pixels.size());

	Buffer stagingBuffer(
		physicalDevice,
		device,
		imageSizeBytes,
		sizeof(unsigned char),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data = nullptr;
	vkMapMemory(device, stagingBuffer.getMemory(), 0, static_cast<VkDeviceSize>(imageSizeBytes), 0, &data);
	memcpy(data, pixels.data(), static_cast<size_t>(imageSizeBytes));
	vkUnmapMemory(device, stagingBuffer.getMemory());

	transitionLayout(
		device,
		queue,
		commandPool,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	uint32_t layerCount = imageSizeBytes / (mExtent.mExtent.width * mExtent.mExtent.height * 4);
	copy(device, queue, commandPool, stagingBuffer, *this, layerCount);

	if (mMipLevels > 1)
	{
		generateMipmaps(
			device,
			queue,
			commandPool
		);
	}
	else
	{
		transitionLayout(
			device,
			queue,
			commandPool,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		);
	}

	stagingBuffer.destroy(device);
}

void Image::destroy(VkDevice device)
{
	vkDestroyImage(device, mHandle, nullptr);
	vkFreeMemory(device, mMemory, nullptr);
}

void Image::generateMipmaps(
    VkDevice device,
    VkQueue queue,
    const CommandPoolPtr& commandPool)
{
    VkCommandBuffer cmd = commandPool->beginSingle(device);

    int32_t mipWidth = static_cast<int32_t>(mExtent.mExtent.width);
    int32_t mipHeight = static_cast<int32_t>(mExtent.mExtent.height);

    for (uint32_t i = 1; i < mMipLevels; i++)
    {
        // Transition previous mip level into a transfer source.
        VkImageMemoryBarrier2 barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

        barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

        barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        barrier.image = mHandle;

        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkDependencyInfo dependency{};
        dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependency.imageMemoryBarrierCount = 1;
        dependency.pImageMemoryBarriers = &barrier;

        vkCmdPipelineBarrier2(cmd, &dependency);

        // Calculate dimensions of next mip level.
        int32_t nextWidth = std::max(1, mipWidth / 2);
        int32_t nextHeight = std::max(1, mipHeight / 2);

        VkImageBlit2 blit{};
        blit.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;

        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;

        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mipWidth, mipHeight, 1};

        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {nextWidth, nextHeight, 1};

        VkBlitImageInfo2 blitInfo{};
        blitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;

        blitInfo.srcImage = mHandle;
        blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        blitInfo.dstImage = mHandle;
        blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        blitInfo.regionCount = 1;
        blitInfo.pRegions = &blit;
        blitInfo.filter = VK_FILTER_LINEAR;

        vkCmdBlitImage2(cmd, &blitInfo);

        // Transition previous mip level to shader-readable.
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;

        barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        barrier.dstAccessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        vkCmdPipelineBarrier2(cmd, &dependency);

        mipWidth = nextWidth;
        mipHeight = nextHeight;
    }

    // Transition final mip level to shader-readable.
    VkImageMemoryBarrier2 finalBarrier{};
    finalBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

    finalBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    finalBarrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

    finalBarrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
    finalBarrier.dstAccessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;

    finalBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    finalBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    finalBarrier.image = mHandle;

    finalBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    finalBarrier.subresourceRange.baseMipLevel = mMipLevels - 1;
    finalBarrier.subresourceRange.levelCount = 1;
    finalBarrier.subresourceRange.baseArrayLayer = 0;
    finalBarrier.subresourceRange.layerCount = 1;

    VkDependencyInfo dependency{};
    dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;

    dependency.imageMemoryBarrierCount = 1;
    dependency.pImageMemoryBarriers = &finalBarrier;

    vkCmdPipelineBarrier2(cmd, &dependency);

    commandPool->endSingle(device, queue, cmd);
}

void Image::transitionLayout(
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
		commandBuffer, mHandle, mMipLevels, 1,
		oldLayout, newLayout,
		srcAccess, srcStage,
		dstAccess, dstStage,
		mAspectFlags);

	commandPool->endSingle(device, queue, commandBuffer);
}

void Image::copy(
	VkDevice device,
	VkQueue queue,
	const CommandPoolPtr& commandPool,
	Buffer& buffer,
	Image& image,
	uint32_t layers) const
{
	VkCommandBuffer commandBuffer = commandPool->beginSingle(device);

	VkDeviceSize layerSize = static_cast<VkDeviceSize>(mExtent.mExtent.width * mExtent.mExtent.height * 4);

	std::vector<VkBufferImageCopy> regions(layers);

	for (uint32_t layer = 0; layer < layers; layer++)
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
			mExtent.mExtent.width,
			mExtent.mExtent.height,
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

Image::Memory Image::getMemoryInfo(
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
