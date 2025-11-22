#include "ResourceRegistryBackend.h"

using namespace ascen;

const BufferPtr& ResourceRegistryBackend::getBuffer(
	const ResourceRegistry& registry, const std::string& name)
{
	if (registry.bufferExists(name))
	{
		return registry.mBuffers.at(name);
	}
	else
	{
		throw std::runtime_error("Buffer does not exist!");
	}
}

const TexturePtr& ResourceRegistryBackend::getTexture(
	const ResourceRegistry& registry, const std::string& name)
{
	if (registry.textureExists(name))
	{
		return registry.mTextures.at(name);
	}
	else
	{
		throw std::runtime_error("Texture does not exist!");
	}
}

const SamplerPtr& ResourceRegistryBackend::getSampler(
	const ResourceRegistry& registry, const std::string& name)
{
	if (registry.samplerExists(name))
	{
		return registry.mSamplers.at(name);
	}
	else
	{
		throw std::runtime_error("Sampler does not exist!");
	}
}

