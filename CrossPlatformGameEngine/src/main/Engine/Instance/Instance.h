#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class Instance
	{
	public:
		static VkInstance create(
			const std::vector<const char*>& validationLayers,
			const std::vector<const char*>& extensions);

		static void destroy(VkInstance instance);
	};

}
