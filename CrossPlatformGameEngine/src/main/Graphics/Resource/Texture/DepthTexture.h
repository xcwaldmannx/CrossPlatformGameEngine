#pragma once

#include "Texture2.h"

namespace ascen
{

	class DepthTexture : public Texture2
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