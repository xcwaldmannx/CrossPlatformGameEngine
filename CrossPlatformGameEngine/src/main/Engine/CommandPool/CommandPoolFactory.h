#pragma once

#include "../Core/Types.h"

#include <cstdint>

#include <vulkan/vulkan.h>

namespace ascen
{
	class CommandPool;

	class CommandPoolFactory
	{
	public:
		CommandPoolFactory(VkDevice device);

		CommandPoolPtr create(uint32_t queueFamilyIndex) const;

	private:
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
