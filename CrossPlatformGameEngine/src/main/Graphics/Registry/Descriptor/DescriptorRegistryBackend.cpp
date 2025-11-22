#include "DescriptorRegistryBackend.h"

using namespace ascen;

const DescriptorSetLayoutPtr& DescriptorRegistryBackend::getDescriptorLayout(
	DescriptorRegistry& registry, const std::string& name)
{
	if (registry.exists(name))
	{
		return registry.mDescriptorSetLayouts.at(name);
	}
}
