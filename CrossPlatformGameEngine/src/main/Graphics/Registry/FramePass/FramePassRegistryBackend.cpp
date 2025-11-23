#include "FramePassRegistryBackend.h"

#include <stdexcept>

using namespace ascen;

const FramePassPtr& FramePassRegistryBackend::getFramePass(
	const FramePassRegistry& registry, const std::string& name)
{
	if (registry.exists(name))
	{
		return registry.mFramePasses.at(name);
	}
	else
	{
		throw std::runtime_error("Frame pass does not exist!");
	}
}

const std::unordered_map<std::string, FramePassPtr>& FramePassRegistryBackend::getFramePasses(
	const FramePassRegistry& registry)
{
	return registry.mFramePasses;
}