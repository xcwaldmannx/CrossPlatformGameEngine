#pragma once

#include "Image_I.h"
#include "../Buffer/Buffer.h"
#include "../../CommandPool/CommandPool.h"

namespace ascen
{

	class Image : public Image_I
	{
	public:
		struct Memory
		{
			VkDeviceSize mSize;
			uint32_t mTypeIndex;
		};

		Image(
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

		static Memory getMemoryInfo(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkImage image,
			VkMemoryPropertyFlags memoryFlags);

	private:
		void transitionLayout(
			VkDevice device,
			VkQueue queue,
			const CommandPoolPtr& commandPool,
			VkImageLayout oldLayout,
			VkImageLayout newLayout);

		void copy(
			VkDevice device,
			VkQueue queue,
			const CommandPoolPtr& commandPool,
			Buffer& buffer,
			Image& image,
			uint32_t layers) const;

	private:
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mLayers;
		VkFormat mFormat;
		VkImageAspectFlags mAspectFlags;
	};

}