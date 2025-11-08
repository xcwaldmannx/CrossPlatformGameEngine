#pragma once

#include "../../CommandPool/CommandPool.h"
#include "../Image/Image.h"

#include <memory>

#include <vulkan/vulkan.h>

namespace ascen
{

	class Texture
	{
	public:
		static Texture create(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkQueue graphicsQueue,
			std::shared_ptr<CommandPool> commandPool,
			VkFormat format,
			VkImageTiling tiling,
			unsigned char* pixels,
			uint32_t width,
			uint32_t height,
			uint32_t layers,
			VkImageUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryFlags,
			VkImageAspectFlags aspectFlags);

		static void destroy(VkDevice device, Texture& texture);

		VkImageView getView() const;

	private:
		Texture(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkQueue graphicsQueue,
			std::shared_ptr<CommandPool> commandPool,
			VkFormat format,
			VkImageTiling tiling,
			unsigned char* pixels,
			uint32_t width,
			uint32_t height,
			uint32_t layers,
			VkImageUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryFlags,
			VkImageAspectFlags aspectFlags);

	private:
		Image mImage{};
		VkImageView mView = VK_NULL_HANDLE;
	};

}
