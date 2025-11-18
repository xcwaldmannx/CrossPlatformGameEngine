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
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<DrawCommand>& drawCommands);
	};

}
