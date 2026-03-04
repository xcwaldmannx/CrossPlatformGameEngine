#pragma once

#include "Vertex_I.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class Vertex : public Vertex_I
	{
	public:
		Vertex(
			const VertexBinding& binding,
			const std::vector<VertexAttribute>& attributes);

		const VertexBinding& getBinding() const override;
		const std::vector<VertexAttribute>& getAttributes() const override;

	private:
		const VertexBinding mBinding;
		const std::vector<VertexAttribute> mAttributes;
	};

}
