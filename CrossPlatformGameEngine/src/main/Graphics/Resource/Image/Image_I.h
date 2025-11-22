#pragma once

#include "../../Handle/Handle.h"

#include <vulkan/vulkan.h>

namespace ascen
{

	class Image_I : public Handle<VkImage>
	{
	public:
		virtual void create(VkDevice device) override = 0;
		virtual void destroy(VkDevice device) override = 0;

		VkDeviceMemory getMemory()
		{
			return mMemory;
		}

	protected:
		VkDeviceMemory mMemory = VK_NULL_HANDLE;
	};

}