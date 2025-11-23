#include "ResourceRegistry.h"

#include "../../Core/VulkanContext.h"
#include "../../Core/RenderContext.h"
#include "../../Resource/Buffer/Buffer.h"
#include "../../Resource/Texture/Texture.h"
#include "../../Resource/Sampler/Sampler.h"

#include <stdexcept>
#include <algorithm>
#include <utility>

using namespace ascen;

ResourceRegistry::ResourceRegistry(
	VulkanContext& vulkanContext,
	RenderContext& renderContext) :
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
		throw std::runtime_error("A buffer with that name already exists!");
	}

	mRegisteredNames.push_back(entry.mName);
	mBufferEntries.emplace_back(std::move(entry));
}

void ResourceRegistry::registerTexture(TextureEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A texture with that name already exists!");
	}

	mRegisteredNames.push_back(entry.mName);
	mTextureEntries.emplace_back(std::move(entry));
}

void ResourceRegistry::registerSampler(SamplerEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A sampler with that name already exists!");
	}

	mRegisteredNames.push_back(entry.mName);
	mSamplerEntries.emplace_back(std::move(entry));
}

void ResourceRegistry::reconstruct()
{
	cleanup();

	for (auto& entry : mBufferEntries)
	{
		switch (entry.mType)
		{
		case BufferType::VERTEX:
			mBuffers.emplace(entry.mName, mBufferFactory.createVertex(
				mCommandPool,
				entry.mCapacity,
				entry.mStride));
			break;
		case BufferType::INDEX:
			mBuffers.emplace(entry.mName, mBufferFactory.createIndex(
				mCommandPool,
				entry.mCapacity,
				entry.mStride));
			break;
		case BufferType::UNIFORM:
			mBuffers.emplace(entry.mName, mBufferFactory.createUniform(
				entry.mCapacity,
				entry.mStride));
			break;
		case BufferType::STORAGE:
			mBuffers.emplace(entry.mName, mBufferFactory.createStorage(
				mCommandPool,
				entry.mCapacity,
				entry.mStride));
			break;
		}
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
		mSamplers.emplace(entry.mName, mSamplerFactory.createSampler());
	}
}

void ResourceRegistry::updateBuffer(
	const std::string& name,
	const void* items,
	uint32_t itemCount,
	uint32_t itemSize,
	uint32_t offset)
{
	if (bufferExists(name))
	{
		mBuffers.at(name)->update(mPhysicalDevice, mDevice, mGraphicsQueue, mCommandPool, items, itemCount, itemSize, offset);
	}
	else
	{
		throw std::runtime_error("Buffer does not exist!");
	}
}

void ResourceRegistry::updateTexture(const std::string& name, const std::vector<unsigned char>& pixels)
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
	return (mBuffers.find(name) != mBuffers.end());
}

bool ResourceRegistry::textureExists(const std::string& name) const
{
	return (mTextures.find(name) != mTextures.end());
}

bool ResourceRegistry::samplerExists(const std::string& name) const
{
	return (mSamplers.find(name) != mSamplers.end());
}

