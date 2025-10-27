#include "ImageHandler.h"

namespace ascen {

	void ImageHandler::allocateImage(
		const ImageCreateInfo& info,
		Image* image) {

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(info.mLogicalDevice->mDevice, image->mImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex =
			findMemoryType(
				info.mPhysicalDevice->mDevice,
				memRequirements.memoryTypeBits,
				info.mProperties);

		if (vkAllocateMemory(info.mLogicalDevice->mDevice, &allocInfo, nullptr, &image->mMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(info.mLogicalDevice->mDevice, image->mImage, image->mMemory, 0);
	}

	void ImageHandler::transitionImageLayout(
		const ImageCreateInfo& info,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		Image* image) {

		VkCommandBuffer commandBuffer = nullptr;
		beginSingleTimeCommands(*info.mLogicalDevice, *info.mCommandPool, &commandBuffer);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image->mImage;
		barrier.subresourceRange.aspectMask = info.mFlags;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = image->mLayers;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		endSingleTimeCommands(*info.mLogicalDevice, info.mGraphicsQueue, *info.mCommandPool, &commandBuffer);
	}

	void ImageHandler::copyBufferToImage(
		const ImageCreateInfo& info,
		Buffer& buffer,
		Image* image)
	{
		VkCommandBuffer commandBuffer = nullptr;
		beginSingleTimeCommands(*info.mLogicalDevice, *info.mCommandPool, &commandBuffer);

		// Calculate total size per image layer
		uint32_t width = static_cast<uint32_t>(image->mWidth);
		uint32_t height = static_cast<uint32_t>(image->mHeight);
		uint32_t layerCount = static_cast<uint32_t>(image->mLayers);  // <-- You need to store this in your Image struct
		VkDeviceSize layerSize = width * height * 4; // 4 bytes per pixel for RGBA8

		std::vector<VkBufferImageCopy> regions(layerCount);

		for (uint32_t layer = 0; layer < layerCount; ++layer) {
			VkBufferImageCopy& region = regions[layer];
			region.bufferOffset = layer * layerSize;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;

			region.imageSubresource.aspectMask = info.mFlags;

			if (info.mFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || info.mFormat == VK_FORMAT_D24_UNORM_S8_UINT) {
				region.imageSubresource.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}

			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = layer;
			region.imageSubresource.layerCount = 1;

			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = {
				width,
				height,
				1
			};
		}

		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer.mBuffer,
			image->mImage,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			static_cast<uint32_t>(regions.size()),
			regions.data());

		endSingleTimeCommands(*info.mLogicalDevice, info.mGraphicsQueue, *info.mCommandPool, &commandBuffer);
	}


	void createImage(const ImageCreateInfo& info, const RawImage& rawImage, Image* image) {
		image->mWidth = rawImage.mWidth;
		image->mHeight = rawImage.mHeight;
		image->mLayers = 1;

		Buffer stagingBuffer;
		VkDeviceSize imageSize = image->mWidth * image->mHeight * 4;

		createBuffer(
			info.mPhysicalDevice->mDevice,
			info.mLogicalDevice->mDevice,
			imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer);

		void* data;
		vkMapMemory(info.mLogicalDevice->mDevice, stagingBuffer.mMemory, 0, imageSize, 0, &data);
		memcpy(data, rawImage.mPixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(info.mLogicalDevice->mDevice, stagingBuffer.mMemory);

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = (uint32_t) rawImage.mWidth;
		imageInfo.extent.height = (uint32_t) rawImage.mHeight;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = (VkFormat) info.mFormat;
		imageInfo.tiling = (VkImageTiling) info.mTiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = info.mUsage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(info.mLogicalDevice->mDevice, &imageInfo, nullptr, &image->mImage) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		ImageHandler::allocateImage(
			info,
			image);

		ImageHandler::transitionImageLayout(
			info,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			image);

		ImageHandler::copyBufferToImage(
			info,
			stagingBuffer,
			image);

		ImageHandler::transitionImageLayout(
			info,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			image);

		destroyBuffer(*info.mLogicalDevice, stagingBuffer);
	}

	void createDepthImage(const DepthImageCreateInfo& info, Image* image) {
		image->mWidth = info.mExtent->width;
		image->mHeight = info.mExtent->height;
		image->mLayers = 1;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = image->mWidth;
		imageInfo.extent.height = image->mHeight;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = image->mLayers;
		imageInfo.format = (VkFormat) info.mImageCreateInfo->mFormat;
		imageInfo.tiling = (VkImageTiling) info.mImageCreateInfo->mTiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = info.mImageCreateInfo->mUsage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(info.mImageCreateInfo->mLogicalDevice->mDevice, &imageInfo, nullptr, &image->mImage) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		ImageHandler::allocateImage(
			*info.mImageCreateInfo,
			image);

	}

	void destroyImage(LogicalDevice logicalDevice, Image& image) {
		vkDestroyImage(logicalDevice.mDevice, image.mImage, nullptr);
		vkFreeMemory(logicalDevice.mDevice, image.mMemory, nullptr);
	}

	void createImages(
		const ImageCreateInfo& info,
		std::vector<RawImage> rawImages,
		Image* image) {

		image->mWidth = rawImages[0].mWidth;
		image->mHeight = rawImages[0].mHeight;
		image->mLayers = rawImages.size();

		std::vector<unsigned char> pixels;
		size_t pixelsPerImage = (image->mWidth * image->mHeight) * 4;

		for (const RawImage& rawImage : rawImages) {
			pixels.insert(pixels.end(), rawImage.mPixels, rawImage.mPixels + pixelsPerImage);
		}

		Buffer stagingBuffer;
		VkDeviceSize imageSize = pixelsPerImage * image->mLayers;

		createBuffer(
			info.mPhysicalDevice->mDevice,
			info.mLogicalDevice->mDevice,
			imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer);

		void* data;
		vkMapMemory(info.mLogicalDevice->mDevice, stagingBuffer.mMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels.data(), static_cast<size_t>(imageSize));
		vkUnmapMemory(info.mLogicalDevice->mDevice, stagingBuffer.mMemory);

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = (uint32_t) image->mWidth;
		imageInfo.extent.height = (uint32_t) image->mHeight;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = image->mLayers;
		imageInfo.format = (VkFormat) info.mFormat;
		imageInfo.tiling = (VkImageTiling) info.mTiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = info.mUsage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(info.mLogicalDevice->mDevice, &imageInfo, nullptr, &image->mImage) != VK_SUCCESS) {
			throw std::runtime_error("failed to create images!");
		}

		ImageHandler::allocateImage(
			info,
			image);

		ImageHandler::transitionImageLayout(
			info,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			image);

		ImageHandler::copyBufferToImage(
			info,
			stagingBuffer,
			image);

		ImageHandler::transitionImageLayout(
			info,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			image);

		destroyBuffer(*info.mLogicalDevice, stagingBuffer);
	}

}