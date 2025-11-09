#pragma once

#include "../Buffer/Buffer.h"
#include "../../CommandPool/CommandPool.h"

#include <memory>

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	struct ImageMemory
	{
		VkDeviceSize mSize;
		uint32_t mTypeIndex;
	};

	class Image
	{
	public:
		void transition();

		static Image create(
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
			VkImageAspectFlags aspectFlags);

		static void destroy(VkDevice device, Image& image);

		static void copy(
			VkDevice device,
			VkQueue graphicsQueue,
			std::shared_ptr<CommandPool> commandPool,
			Buffer& buffer,
			Image& image,
			VkFormat format,
			uint32_t width,
			uint32_t height,
			uint32_t layers,
			VkImageAspectFlags aspectFlags);

		static ImageMemory getMemory(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkImage image,
			VkMemoryPropertyFlags memoryFlags);

	private:
		Image() = default;

		Image(
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
			VkImageAspectFlags aspectFlags);

		void transitionLayout(
			VkDevice device,
			VkQueue graphicsQueue,
			std::shared_ptr<CommandPool> commandPool,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			uint32_t layers,
			VkImageAspectFlags aspectFlags);

	private:
		VkImage mImage = VK_NULL_HANDLE;
		VkDeviceMemory mMemory = VK_NULL_HANDLE;

		friend class Texture;
	};

}