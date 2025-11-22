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
			const CommandPoolPtr& commandPool,
			uint32_t itemCount,
			uint32_t itemSize);
	};

}
