#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	using VertexBinding = VkVertexInputBindingDescription;
	using VertexAttribute = VkVertexInputAttributeDescription;

	struct Vertex_I
	{
		virtual const VertexBinding& getBinding() const = 0;
		virtual const std::vector<VertexAttribute>& getAttributes() const = 0;
	};

}
