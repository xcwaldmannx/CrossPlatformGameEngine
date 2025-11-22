#pragma once

#include "Buffer2.h"

#include "../../Types.h"

#include <vector>

namespace ascen
{

	class UniformBuffer : public Buffer2
	{
	public:
		UniformBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint64_t itemCount,
			uint64_t itemSize);
	};

}
