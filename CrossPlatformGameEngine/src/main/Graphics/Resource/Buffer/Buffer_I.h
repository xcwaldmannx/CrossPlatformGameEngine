#pragma once

#include "../../Handle/Handle.h"

#include <vulkan/vulkan.h>

namespace ascen
{

	class Buffer_I : public Handle<VkBuffer>
	{
	public:
		virtual void create(VkDevice device) override = 0;
		virtual void destroy(VkDevice device) override = 0;

		VkDeviceMemory getMemory() const
		{
			return mMemory;
		}

		void* getMappedMemory() const
		{
			return mMappedMemory;
		}

	protected:
		VkDeviceMemory mMemory = VK_NULL_HANDLE;
		void* mMappedMemory = nullptr;
	};

}