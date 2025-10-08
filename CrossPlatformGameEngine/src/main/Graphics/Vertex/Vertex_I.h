#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	struct Vertex_I
	{
		static VkVertexInputBindingDescription getBindingDescription();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
	};

}
