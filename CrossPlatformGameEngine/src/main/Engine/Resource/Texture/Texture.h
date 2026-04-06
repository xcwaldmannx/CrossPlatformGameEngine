#pragma once

#include "../Image/Image.h"

namespace ascen
{

	class Texture : public Handle<VkImageView>
	{
	public:
		Texture(
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
			VkImageAspectFlags aspectFlags);

		void update(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkQueue queue,
			const CommandPoolPtr& commandPool,
			const std::vector<unsigned char>& pixels);

		void destroy(VkDevice device) override;

	protected:
		Image mImage;
	};

}