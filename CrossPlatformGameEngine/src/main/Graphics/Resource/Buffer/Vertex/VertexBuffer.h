#pragma once

#include "../Buffer.h"
#include "../../../Core/Types.h"

namespace ascen
{

	class VertexBuffer : public Buffer
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
