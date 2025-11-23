#pragma once

#include "Sampler_I.h"

namespace ascen
{

	class Sampler : public Sampler_I
	{
	public:
		Sampler(VkPhysicalDevice physicalDevice, VkDevice device);

		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

	};

}