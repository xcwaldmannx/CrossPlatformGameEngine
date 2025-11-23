#pragma once

#include "../Texture.h"

namespace ascen
{

	class DepthTexture : public Texture
	{
	public:
		DepthTexture(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			const CommandPoolPtr& commandPool,
			uint32_t width,
			uint32_t height);
	};

}