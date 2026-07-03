#pragma once

#include "../Core/Types.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	struct Vertex_I
	{
		virtual const VertexBinding& getBinding() const = 0;
		virtual const std::vector<VertexAttribute>& getAttributes() const = 0;
	};

}
