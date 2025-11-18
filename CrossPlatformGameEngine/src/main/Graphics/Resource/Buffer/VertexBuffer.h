#pragma once

#include "Buffer2.h"

#include "../../Types.h"

#include <vector>

namespace ascen
{

	class VertexBuffer : public Buffer2
	{
	public:
		VertexBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<float>& vertices);
	};

}
