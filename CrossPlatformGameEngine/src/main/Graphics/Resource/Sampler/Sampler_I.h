#pragma once

#include "../../HandleManager/Handle.h"

#include <vulkan/vulkan.h>

namespace ascen
{

	class Sampler_I : public Handle<VkSampler>
	{
	public:
		virtual void create(VkDevice device) override = 0;
		virtual void destroy(VkDevice device) override = 0;
	};

}
