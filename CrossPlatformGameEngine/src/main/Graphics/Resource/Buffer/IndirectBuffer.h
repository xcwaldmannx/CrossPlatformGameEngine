#pragma once

#include "Buffer2.h"

#include "../../Types.h"

#include <vector>

namespace ascen
{

	class IndirectBuffer : public Buffer2
	{
	public:
		using DrawCommand = VkDrawIndexedIndirectCommand;

		IndirectBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			const CommandPoolPtr& commandPool,
			uint32_t itemCount);
	};

}
