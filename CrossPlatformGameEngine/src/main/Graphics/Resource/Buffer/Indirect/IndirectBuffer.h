#pragma once

#include "../Buffer.h"
#include "../../../Core/Types.h"

namespace ascen
{

	class IndirectBuffer : public Buffer
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
