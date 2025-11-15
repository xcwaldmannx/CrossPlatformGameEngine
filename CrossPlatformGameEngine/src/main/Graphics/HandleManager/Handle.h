#pragma once

#include "Handle_I.h"

#include <vulkan/vulkan.h>

namespace ascen
{

	template<typename T>
	class Handle : public Handle_I
	{
	public:
		virtual ~Handle() override = default;

		virtual void create(VkDevice device) override = 0;
		virtual void destroy(VkDevice device) override = 0;

		T& handle()
		{
			return mHandle;
		}

	protected:
		T mHandle = VK_NULL_HANDLE;
	};

}
