#pragma once

#include "FramePassRegistry.h"
#include "../../Types.h"

#include <string>
#include <unordered_map>

namespace ascen
{

	class FramePassRegistryBackend
	{
	public:
		static const FramePassPtr& getFramePass(const FramePassRegistry& registry, const std::string& name);
		static const std::unordered_map<std::string, FramePassPtr>& getFramePasses(const FramePassRegistry& registry);
	};

}
