#pragma once

#include "Handle_I.h"

#include <vulkan/vulkan.h>

namespace ascen
{

	template<typename T>
	class Handle : public Handle_I
	{
	public:
		Handle() noexcept = default;
		virtual ~Handle() override = default;

		Handle(const Handle&) = delete;
		Handle& operator=(const Handle&) = delete;

		virtual void destroy(VkDevice device) override = 0;

		T& handle()
		{
			return mHandle;
		}

	protected:
		T mHandle = VK_NULL_HANDLE;
	};

}
