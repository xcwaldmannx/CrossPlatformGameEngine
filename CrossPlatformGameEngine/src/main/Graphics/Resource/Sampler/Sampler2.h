#pragma once

#include "Sampler_I.h"

namespace ascen
{

	class Sampler2 : public Sampler_I
	{
	public:
		Sampler2(VkPhysicalDevice physicalDevice, VkDevice device);

		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

	};

}