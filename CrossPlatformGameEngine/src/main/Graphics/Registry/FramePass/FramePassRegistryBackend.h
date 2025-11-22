#pragma once

#include "FramePassRegistry.h"
#include "../../Types.h"

#include <string>

namespace ascen
{

	class FramePassRegistryBackend
	{
	public:
		static const FramePassPtr& getFramePass(const FramePassRegistry& registry, const std::string& name);
	};

}
