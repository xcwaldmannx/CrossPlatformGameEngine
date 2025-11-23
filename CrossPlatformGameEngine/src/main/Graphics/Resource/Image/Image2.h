#pragma once

#include "Image_I.h"
#include "../Buffer/Buffer2.h"
#include "../../CommandPool/CommandPool.h"

namespace ascen
{

	class Image2 : public Image_I
	{
	public:
		struct Memory
		{
			VkDeviceSize mSize;
			uint32_t mTypeIndex;
		};

		Image2(
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

		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

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
			Buffer2& buffer,
			Image2& image,
			uint32_t layers);

		Memory getMemoryInfo(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkImage image,
			VkMemoryPropertyFlags memoryFlags);

	private:
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mLayers;
		VkFormat mFormat;
		VkImageAspectFlags mAspectFlags;
	};

}