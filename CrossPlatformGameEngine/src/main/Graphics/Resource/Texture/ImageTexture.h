#pragma once

#include "Texture2.h"

namespace ascen
{

	class ImageTexture : public Texture2
	{
	public:
		ImageTexture(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<unsigned char>& pixels,
			uint32_t width,
			uint32_t height,
			uint32_t layers);
	};

}