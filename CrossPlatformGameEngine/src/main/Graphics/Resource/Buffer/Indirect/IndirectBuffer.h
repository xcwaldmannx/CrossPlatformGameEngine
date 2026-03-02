#pragma once

#include "../Buffer.h"
#include "../../../Core/Types.h"

namespace ascen
{

	class IndirectBuffer : public Buffer
	{
	public:
		using IndexedIndirectCommand = VkDrawIndexedIndirectCommand;
		using IndirectCommand        = VkDrawIndirectCommand;

		IndirectBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			const CommandPoolPtr& commandPool,
			uint32_t itemCount,
			uint32_t itemSize);
	};

}
