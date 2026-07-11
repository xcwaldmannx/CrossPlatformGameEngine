#pragma once

#include "../Handle/Handle.h"
#include "../Core/Types.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	struct Vertex_I : public Handle<void*>
	{
		virtual const VertexBinding& getBinding() const = 0;
		virtual const std::vector<VertexAttribute>& getAttributes() const = 0;
		void destroy(VkDevice device) override {}
	};

}
