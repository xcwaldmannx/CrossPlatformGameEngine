#pragma once

#include "DescriptorRegistry.h"

namespace ascen
{

	class DescriptorRegistryBackend
	{
	public:
		static const DescriptorSetLayoutPtr& getDescriptorSetLayout(
			const DescriptorRegistry& registry, const std::string& name);

		static const DescriptorSetPtr& getDescriptorSet(
			const DescriptorRegistry& registry, const std::string& name);
	};

}
