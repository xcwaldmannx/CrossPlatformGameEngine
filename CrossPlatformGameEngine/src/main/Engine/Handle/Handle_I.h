#pragma once

#include <vulkan/vulkan.h>

namespace ascen
{

	class Handle_I
	{
	public:
		virtual ~Handle_I() = default;

		virtual void destroy(VkDevice device) = 0;
	};

}
