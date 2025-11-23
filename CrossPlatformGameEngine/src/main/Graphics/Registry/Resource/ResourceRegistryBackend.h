#pragma once

#include "ResourceRegistry.h"
#include "../../Types.h"

namespace ascen
{

	class ResourceRegistryBackend
	{
	public:
		static const BufferPtr& getBuffer(const ResourceRegistry& registry, const std::string& name);
		static const TexturePtr& getTexture(const ResourceRegistry& registry, const std::string& name);
		static const SamplerPtr& getSampler(const ResourceRegistry& registry, const std::string& name);
	};

}
