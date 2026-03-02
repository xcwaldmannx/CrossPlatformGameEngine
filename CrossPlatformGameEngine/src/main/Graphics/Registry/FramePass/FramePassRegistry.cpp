#include "FramePassRegistry.h"

#include <stdexcept>

#include "../../FrameGraph/Graphics/GraphicsFramePass.h"
#include "../../FrameGraph/Compute/ComputeFramePass.h"
#include "../Resource/ResourceRegistryBackend.h"
#include "../Descriptor/DescriptorRegistryBackend.h"
#include "../Pipeline/PipelineRegistryBackend.h"
#include "../../Resource/Buffer/Buffer.h"
#include "../../Resource/Texture/Texture.h"
#include "../../Pipeline/GraphicsPipeline/GraphicsPipeline_I.h"
#include "../../Pipeline/ComputePipeline/ComputePipeline_I.h"

using namespace ascen;

FramePassRegistry::FramePassRegistry(
	const ResourceRegistry& resourceRegistry,
	const DescriptorRegistry& descriptorRegistry,
	const PipelineRegistry& pipelineRegistry) :
	mResourceRegistry(resourceRegistry),
	mDescriptorRegistry(descriptorRegistry),
	mPipelineRegistry(pipelineRegistry) {}

void FramePassRegistry::registerGraphics(GraphicsFramePassEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A frame pass with that name already exists!");
	}

	mGraphicsEntries.emplace_back(std::move(entry));
}

void FramePassRegistry::registerCompute(ComputeFramePassEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A frame pass with that name already exists!");
	}

	mComputeEntries.emplace_back(std::move(entry));
}

void FramePassRegistry::reconstruct()
{
	cleanup();

	for (const auto& entry : mGraphicsEntries)
	{
		std::vector<VkBuffer> vertexBufferHandles;

		for (auto& vertexBuffer : entry.mVertexBuffers)
		{
			const auto& buffer = ResourceRegistryBackend::getBuffer(mResourceRegistry, vertexBuffer);
			vertexBufferHandles.push_back(buffer->handle());
		}

		VkBuffer indexBufferHandle = VK_NULL_HANDLE;

		if (!entry.mIndexBuffer.empty())
		{
			const auto& indexBuffer = ResourceRegistryBackend::getBuffer(mResourceRegistry, entry.mIndexBuffer);
			indexBufferHandle = indexBuffer->handle();
		}

		std::vector<VkBuffer> readBufferHandles;

		for (auto& readBuffer : entry.mReadBuffers)
		{
			const auto& buffer = ResourceRegistryBackend::getBuffer(mResourceRegistry, readBuffer);
			readBufferHandles.push_back(buffer->handle());
		}

		std::vector<VkBuffer> writeBufferHandles;

		for (auto& writeBuffer : entry.mWriteBuffers)
		{
			const auto& buffer = ResourceRegistryBackend::getBuffer(mResourceRegistry, writeBuffer);
			writeBufferHandles.push_back(buffer->handle());
		}

		std::vector<VkImageView> readTextureHandles;

		for (auto& readTexture : entry.mReadTextures)
		{
			const auto& texture = ResourceRegistryBackend::getTexture(mResourceRegistry, readTexture);
			readTextureHandles.push_back(texture->handle());
		}

		std::vector<VkImageView> writeTextureHandles;

		for (auto& writeTexture : entry.mWriteTextures)
		{
			const auto& texture = ResourceRegistryBackend::getTexture(mResourceRegistry, writeTexture);
			writeTextureHandles.push_back(texture->handle());
		}

		std::vector<VkDescriptorSet> descriptorSetHandles;

		for (auto& descriptorSet : entry.mDescriptorSets)
		{
			const auto& set = DescriptorRegistryBackend::getDescriptorSet(mDescriptorRegistry, descriptorSet);
			descriptorSetHandles.push_back(set->handle());
		}

		const auto& pipeline = PipelineRegistryBackend::getGraphicsPipeline(mPipelineRegistry, entry.mPipeline);
		VkPipeline pipelineHandle = pipeline->handle();
		VkPipelineLayout pipelineLayoutHandle = pipeline->getLayout();

		mFramePasses[entry.mName] = std::make_shared<GraphicsFramePass>(
			FramePassType::GRAPHICS,
			entry.mFramePassMode,
			descriptorSetHandles,
			pipelineHandle,
			pipelineLayoutHandle,
			readBufferHandles,
			writeBufferHandles,
			readTextureHandles,
			writeTextureHandles,
			vertexBufferHandles,
			indexBufferHandle);
	}

	for (const auto& entry : mComputeEntries)
	{
		std::vector<VkBuffer> readBufferHandles;

		for (auto& readBuffer : entry.mReadBuffers)
		{
			const auto& buffer = ResourceRegistryBackend::getBuffer(mResourceRegistry, readBuffer);
			readBufferHandles.push_back(buffer->handle());
		}

		std::vector<VkBuffer> writeBufferHandles;

		for (auto& writeBuffer : entry.mWriteBuffers)
		{
			const auto& buffer = ResourceRegistryBackend::getBuffer(mResourceRegistry, writeBuffer);
			writeBufferHandles.push_back(buffer->handle());
		}

		std::vector<VkImageView> readTextureHandles;

		for (auto& readTexture : entry.mReadTextures)
		{
			const auto& texture = ResourceRegistryBackend::getTexture(mResourceRegistry, readTexture);
			readTextureHandles.push_back(texture->handle());
		}

		std::vector<VkImageView> writeTextureHandles;

		for (auto& writeTexture : entry.mWriteTextures)
		{
			const auto& texture = ResourceRegistryBackend::getTexture(mResourceRegistry, writeTexture);
			writeTextureHandles.push_back(texture->handle());
		}

		std::vector<VkDescriptorSet> descriptorSetHandles;

		for (auto& descriptorSet : entry.mDescriptorSets)
		{
			const auto& set = DescriptorRegistryBackend::getDescriptorSet(mDescriptorRegistry, descriptorSet);
			descriptorSetHandles.push_back(set->handle());
		}

		const auto& pipeline = PipelineRegistryBackend::getComputePipeline(mPipelineRegistry, entry.mPipeline);
		VkPipeline pipelineHandle = pipeline->handle();
		VkPipelineLayout pipelineLayoutHandle = pipeline->getLayout();

		mFramePasses[entry.mName] = std::make_shared<ComputeFramePass>(
			FramePassType::COMPUTE,
			descriptorSetHandles,
			pipelineHandle,
			pipelineLayoutHandle,
			readBufferHandles,
			writeBufferHandles,
			readTextureHandles,
			writeTextureHandles,
			entry.mGroups);
	}
}

void FramePassRegistry::cleanup()
{
	mFramePasses.clear();
}

bool FramePassRegistry::isRegistered(const std::string& name) const
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

bool FramePassRegistry::exists(const std::string& name) const
{
	return (mFramePasses.find(name) != mFramePasses.end());
}
