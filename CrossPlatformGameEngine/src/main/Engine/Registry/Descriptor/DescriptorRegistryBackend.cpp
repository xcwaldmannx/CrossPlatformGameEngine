#include "DescriptorRegistryBackend.h"

using namespace ascen;

const DescriptorSetLayoutPtr& DescriptorRegistryBackend::getDescriptorSetLayout(
	const DescriptorRegistry& registry, const std::string& name)
{
	if (registry.exists(name))
	{
		return registry.mDescriptorSetLayouts.at(name);
	}

	return nullptr;
}

const DescriptorSetPtr& DescriptorRegistryBackend::getDescriptorSet(
	const DescriptorRegistry& registry, const std::string& name)
{
	if (registry.exists(name))
	{
		return registry.mDescriptorSets.at(name);
	}

	return nullptr;
}
