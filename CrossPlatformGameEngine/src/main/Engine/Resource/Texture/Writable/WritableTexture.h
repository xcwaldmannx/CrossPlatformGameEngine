#pragma once

#include "../Texture.h"

namespace ascen
{

	class WritableTexture : public Texture
	{
	public:
		WritableTexture(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			const CommandPoolPtr& commandPool,
			const Format format,
			dim::Extent2D extent,
			uint32_t layers);
	};

}