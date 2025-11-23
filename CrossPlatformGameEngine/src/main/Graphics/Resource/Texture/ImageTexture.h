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
			const CommandPoolPtr& commandPool,
			uint32_t width,
			uint32_t height,
			uint32_t layers);
	};

}