#pragma once

#include "../Texture.h"

namespace ascen
{

	class ImageTexture : public Texture
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