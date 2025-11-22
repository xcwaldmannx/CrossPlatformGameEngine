#pragma once

#include "DescriptorRegistry.h"

namespace ascen
{

	class DescriptorRegistryBackend
	{
	public:
		static const DescriptorSetLayoutPtr& getDescriptorLayout(
			DescriptorRegistry& registry, const std::string& name);
	};

}
