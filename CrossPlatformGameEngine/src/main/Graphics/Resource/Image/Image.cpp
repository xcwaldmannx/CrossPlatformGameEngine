#include "Image.h"

#include <stdexcept>

using namespace ascen;

void Image::transition() // abstract transitions somehow
{

}

Image Image::create(
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
	return Image(
		physicalDevice, device,
		graphicsQueue, commandPool,
		format, tiling,
		pixels, width, height, layers,
		usageFlags, memoryFlags, aspectFlags);
}

void Image::destroy(VkDevice device, Image& image)
{
	vkDestroyImage(device, image.mImage, nullptr);
	vkFreeMemory(device, image.mMemory, nullptr);
}

void Image::copy(
	VkDevice device,
	VkQueue graphicsQueue,
	std::shared_ptr<CommandPool> commandPool,
	Buffer& buffer,
	Image& image,
	VkFormat format,
	uint32_t width,
	uint32_t height,
	uint32_t layers,
	VkImageAspectFlags aspectFlags)
{
	VkCommandBuffer commandBuffer = nullptr;
	commandPool->beginSingleTimeCommands(device, &commandBuffer);

	// Calculate total size per image layer
	uint32_t layerCount = layers;
	VkDeviceSize layerSize = width * height * 4;

	std::vector<VkBufferImageCopy> regions(layerCount);

	for (uint32_t layer = 0; layer < layerCount; layer++)
	{
		VkBufferImageCopy& region = regions[layer];
		region.bufferOffset = layer * layerSize;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = aspectFlags;

		if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
		{
			region.imageSubresource.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = layer;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent =
		{
			width,
			height,
			1
		};
	}

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer.mBuffer,
		image.mImage,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		static_cast<uint32_t>(regions.size()),
		regions.data());

	commandPool->endSingleTimeCommands(device, graphicsQueue, &commandBuffer);
}

ImageMemory Image::getMemory(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkImage image,
	VkMemoryPropertyFlags memoryFlags)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	ImageMemory mem{};

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

Image::Image(
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
	Buffer* stagingBuffer = nullptr;

	if (!pixels.empty())
	{
		VkDeviceSize pixelCount = width * height * layers;
		size_t pixelSize = 4;
		VkDeviceSize imageSizeBytes = pixelCount * pixelSize;

		stagingBuffer = new Buffer(
			physicalDevice,
			device,
			pixelCount,
			pixelSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data;
		vkMapMemory(device, stagingBuffer->mMemory, 0, imageSizeBytes, 0, &data);
		memcpy(data, pixels.data(), static_cast<size_t>(imageSizeBytes));
		vkUnmapMemory(device, stagingBuffer->mMemory);
	}

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(width);
	imageInfo.extent.height = static_cast<uint32_t>(height);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = layers;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usageFlags;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(device, &imageInfo, nullptr, &mImage) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	ImageMemory mem = getMemory(physicalDevice, device, mImage, memoryFlags);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = mem.mSize;
	allocInfo.memoryTypeIndex = mem.mTypeIndex;

	if (vkAllocateMemory(device, &allocInfo, nullptr, &mMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device, mImage, mMemory, 0);

	if (!pixels.empty())
	{
		transitionLayout(
			device,
			graphicsQueue,
			commandPool,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			layers,
			aspectFlags);

		copy(device, graphicsQueue, commandPool, *stagingBuffer, *this, format, width, height, layers, aspectFlags);

		transitionLayout(
			device,
			graphicsQueue,
			commandPool,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			layers,
			aspectFlags);

		Buffer::destroy(device, *stagingBuffer);

		delete stagingBuffer;
	}
}

void Image::transitionLayout(
	VkDevice device,
	VkQueue graphicsQueue,
	std::shared_ptr<CommandPool> commandPool,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	uint32_t layers,
	VkImageAspectFlags aspectFlags)
{
	VkCommandBuffer commandBuffer = nullptr;
	commandPool->beginSingleTimeCommands(device, &commandBuffer);

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = mImage;
	barrier.subresourceRange.aspectMask = aspectFlags;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layers;

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

	commandPool->endSingleTimeCommands(device, graphicsQueue, &commandBuffer);
}
