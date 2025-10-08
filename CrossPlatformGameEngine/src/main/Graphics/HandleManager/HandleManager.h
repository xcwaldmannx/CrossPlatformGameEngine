#pragma once

#include "Handle_I.h"

#include <concepts>
#include <memory>

#include <vector>

namespace ascen
{

	class HandleManager
	{
	public:
		void create(VkDevice device, Handle_I* handle);
		void destroy(VkDevice device);

	private:
		std::vector<Handle_I*> mHandles;
	};

}
