#pragma once

#include "../../Handle/Handle.h"

#include <vulkan/vulkan.h>

namespace ascen
{

	class Image_I : public Handle<VkImage>
	{
	public:
		void destroy(VkDevice device) override = 0;

		VkDeviceMemory getMemory() const
		{
			return mMemory;
		}

	protected:
		VkDeviceMemory mMemory = VK_NULL_HANDLE;
	};

}