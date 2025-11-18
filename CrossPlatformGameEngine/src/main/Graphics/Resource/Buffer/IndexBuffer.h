#pragma once

#include "Buffer2.h"

#include "../../Types.h"

#include <vector>

namespace ascen
{

	class IndexBuffer : public Buffer2
	{
	public:
		IndexBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<uint32_t>& indices);
	};

}
