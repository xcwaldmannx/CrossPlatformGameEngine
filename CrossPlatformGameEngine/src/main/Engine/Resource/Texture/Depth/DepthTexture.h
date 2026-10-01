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
			dim::Extent2D extent);
	};

}