#pragma once

#include "../../Core/Types.h"

#include <cstdint>
#include <memory>

#include <vulkan/vulkan.h>

namespace ascen
{

	class TextureFactory
	{
	public:
		TextureFactory(
			VkPhysicalDevice physicalDevice,
			VkDevice device);

		TexturePtr createImage(
			const CommandPoolPtr& commandPool,
			dim::Extent2D extent,
			uint32_t layers) const;

		TexturePtr createWritable(
			const CommandPoolPtr& commandPool,
			const Format format,
			dim::Extent2D extent,
			uint32_t layers) const;

		TexturePtr createDepth(
			const CommandPoolPtr& commandPool,
			dim::Extent2D extent) const;

	private:
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
