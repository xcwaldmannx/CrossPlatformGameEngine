#pragma once

#include "../Buffer.h"
#include "../../../Core/Types.h"

namespace ascen
{

	class UniformBuffer : public Buffer
	{
	public:
		UniformBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint64_t itemCount,
			uint64_t itemSize);
	};

}
