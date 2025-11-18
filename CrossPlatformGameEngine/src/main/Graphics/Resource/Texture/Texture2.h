#pragma once

#include "../Image/Image2.h"

namespace ascen
{

	class Texture2 : public Handle<VkImageView>
	{
	public:
		Texture2(
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

		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

	protected:
		Image2 mImage;
	};

}