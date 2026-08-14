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
			uint32_t width,
			uint32_t height,
			uint32_t layers);
	};

}