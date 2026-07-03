#include "ResourceRegistry.h"

#include "../../Core/VulkanContext.h"
#include "../../Core/RenderContext.h"
#include "../../Resource/Texture/Texture.h"
#include "../../Resource/Sampler/Sampler.h"

#include <stdexcept>
#include <algorithm>
#include <utility>

using namespace ascen;

ResourceRegistry::ResourceRegistry(
	const VulkanContext& vulkanContext,
	const RenderContext& renderContext) :
	mPhysicalDevice(vulkanContext.getPhysicalDevice()),
	mDevice(vulkanContext.getDevice()),
	mGraphicsQueue(vulkanContext.getGraphicsQueue()),
	mCommandPool(renderContext.getCommandPool()),
	mBufferFactory(vulkanContext.getBufferFactory()),
	mTextureFactory(vulkanContext.getTextureFactory()),
	mSamplerFactory(vulkanContext.getSamplerFactory()) {}

void ResourceRegistry::registerBuffer(BufferEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A resource with that name already exists!");
	}

	mRegisteredNames.push_back(entry.mName);
	mBufferEntries.emplace_back(std::move(entry));
}

void ResourceRegistry::registerTexture(TextureEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A resource with that name already exists!");
	}

	mRegisteredNames.push_back(entry.mName);
	mTextureEntries.emplace_back(std::move(entry));
}

void ResourceRegistry::registerSampler(SamplerEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A resource with that name already exists!");
	}

	mRegisteredNames.push_back(entry.mName);
	mSamplerEntries.emplace_back(std::move(entry));
}

void ResourceRegistry::reconstruct()
{
	cleanup();

	for (const auto& entry : mBufferEntries)
	{
		mBuffers.emplace(entry.mName, mBufferFactory.create(
			entry.mCapacity,
			entry.mStride,
			entry.mUsageFlags,
			entry.mMemoryFlags));
	}

	for (auto& entry : mTextureEntries)
	{
		switch (entry.mType)
		{
		case TextureType::IMAGE:
			mTextures.emplace(entry.mName, mTextureFactory.createImage(
				mCommandPool,
				entry.mWidth,
				entry.mHeight,
				entry.mLayers));
			break;
		case TextureType::DEPTH:
			mTextures.emplace(entry.mName, mTextureFactory.createDepth(
				mCommandPool,
				entry.mWidth,
				entry.mHeight));
			break;
		}
	}

	for (auto& entry : mSamplerEntries)
	{
		mSamplers.emplace(entry.mName, mSamplerFactory.create());
	}
}

void ResourceRegistry::uploadBuffer(
	const std::string& name,
	const void* items,
	uint32_t itemCount,
	uint32_t itemSize,
	uint32_t offset) const
{
	if (bufferExists(name))
	{
		mBuffers.at(name)->upload(mPhysicalDevice, mDevice, mGraphicsQueue, mCommandPool, items, itemCount, itemSize, offset);
	}
	else
	{
		throw std::runtime_error("Buffer does not exist!");
	}
}

void ResourceRegistry::uploadTexture(const std::string& name, const std::vector<unsigned char>& pixels) const
{
	if (textureExists(name))
	{
		mTextures.at(name)->update(mPhysicalDevice, mDevice, mGraphicsQueue, mCommandPool, pixels);
	}
	else
	{
		throw std::runtime_error("Texture does not exist!");
	}
}

void ResourceRegistry::cleanup()
{
	for (auto& [name, buffer] : mBuffers)
	{
		if (buffer) mBuffers.at(name)->destroy(mDevice);
	}
	mBuffers.clear();

	for (auto& [name, texture] : mTextures)
	{
		if (texture) mTextures.at(name)->destroy(mDevice);
	}
	mTextures.clear();

	for (auto& [name, sampler] : mSamplers)
	{
		if (sampler) mSamplers.at(name)->destroy(mDevice);
	}
	mSamplers.clear();
}

bool ResourceRegistry::isRegistered(const std::string& name) const
{
	for (const auto& registeredName : mRegisteredNames)
	{
		if (name == registeredName)
		{
			return true;
		}
	}

	return false;
}

bool ResourceRegistry::bufferExists(const std::string& name) const
{
	return (mBuffers.contains(name));
}

bool ResourceRegistry::textureExists(const std::string& name) const
{
	return (mTextures.contains(name));
}

bool ResourceRegistry::samplerExists(const std::string& name) const
{
	return (mSamplers.contains(name));
}

