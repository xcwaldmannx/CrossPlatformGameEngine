#pragma once

#include <vulkan/vulkan.h>

namespace ascen
{

	class Sampler
	{
	public:
		static Sampler create(VkPhysicalDevice physicalDevice, VkDevice device);

		static void destroy(VkDevice device, Sampler& sampler);

		const VkSampler& getSampler() const;

	private:
		Sampler(VkPhysicalDevice physicalDevice, VkDevice device);

	private:
		VkSampler mSampler;
	};

}