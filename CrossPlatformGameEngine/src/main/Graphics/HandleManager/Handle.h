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

		virtual void create(VkDevice device) override;
		virtual void destroy(VkDevice device) override;

		T handle() const
		{
			return mHandle;
		}

		operator T() const
		{
			return mHandle;
		}



	protected:
		T mHandle = VK_NULL_HANDLE;
	};

}
