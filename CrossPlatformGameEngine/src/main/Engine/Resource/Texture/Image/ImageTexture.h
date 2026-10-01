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
			dim::Extent2D extent,
			uint32_t layers);
	};

}