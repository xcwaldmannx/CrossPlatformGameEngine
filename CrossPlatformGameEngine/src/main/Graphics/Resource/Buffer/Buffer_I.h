#pragma once

#include "../../HandleManager/Handle.h"

#include <vulkan/vulkan.h>

namespace ascen
{

	class Buffer_I : public Handle<VkBuffer>
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
		void* mMappedMemory = nullptr;
	};

}