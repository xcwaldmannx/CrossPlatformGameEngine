#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	using VertexBinding = VkVertexInputBindingDescription;
	using VertexAttribute = VkVertexInputAttributeDescription;

	class Vertex
	{
	public:
		Vertex(
			const VertexBinding& binding,
			const std::vector<VertexAttribute>& attributes);

		const VertexBinding& getBinding() const;
		const std::vector<VertexAttribute>& getAttributes() const;

	private:
		const VertexBinding mBinding;
		const std::vector<VertexAttribute> mAttributes;
	};

}
