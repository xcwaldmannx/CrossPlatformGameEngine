#pragma once

#include "Sampler_I.h"

namespace ascen
{

	class Sampler : public Sampler_I
	{
	public:
		Sampler(VkPhysicalDevice physicalDevice, VkDevice device);

		void destroy(VkDevice device) override;

	};

}