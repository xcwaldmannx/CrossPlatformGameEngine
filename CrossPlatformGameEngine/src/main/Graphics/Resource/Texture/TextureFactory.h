#pragma once

#include "ImageTexture.h"
#include "DepthTexture.h"
#include "../../Types.h"

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
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<unsigned char>& pixels,
			uint32_t width,
			uint32_t height,
			uint32_t layers) const;

		TexturePtr createDepth(
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			uint32_t width,
			uint32_t height) const;

	private:
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
