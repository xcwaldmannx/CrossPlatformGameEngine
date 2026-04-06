#include "PipelineRegistry.h"

#include "../Vertex/VertexRegistryBackend.h"
#include "../Descriptor/DescriptorRegistryBackend.h"
#include "../../Pipeline/PushConstant/PushConstant.h"
#include "../../Pipeline/GraphicsPipeline/GraphicsPipeline_I.h"
#include "../../Pipeline/ComputePipeline/ComputePipeline_I.h"

#include <cstring>
#include <stdexcept>

using namespace ascen;

PipelineRegistry::PipelineRegistry(
	const VulkanContext& vulkanContext,
	const RenderContext& renderContext,
	const VertexRegistry& vertexRegistry,
	const DescriptorRegistry& descriptorRegistry) :
	mDevice(vulkanContext.getDevice()),
	mGraphicsQueue(vulkanContext.getGraphicsQueue()),
	mCommandPool(renderContext.getCommandPool()),
	mPushConstantFactory(vulkanContext.getPushConstantFactory()),
	mGraphicsPipelineFactory(vulkanContext.getGraphicsPipelineFactory()),
	mComputePipelineFactory(vulkanContext.getComputePipelineFactory()),
	mSwapchain(renderContext.getSwapchain()),
	mRenderPass(renderContext.getRenderPass()),
	mVertexRegistry(vertexRegistry),
	mDescriptorRegistry(descriptorRegistry) {}

void PipelineRegistry::registerGraphicsPipeline(GraphicsPipelineEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A resource with that name already exists!");
	}

	mRegisteredNames.push_back(entry.mName);
	mGraphicsEntries.emplace_back(std::move(entry));
}

void PipelineRegistry::registerComputePipeline(ComputePipelineEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A resource with that name already exists!");
	}

	mRegisteredNames.push_back(entry.mName);
	mComputeEntries.emplace_back(std::move(entry));
}

void PipelineRegistry::pushConstants(
	const std::string& pipelineName,
	const std::string& pushConstantName,
	const void* data)
{
	std::string name = pipelineName + "_" + pushConstantName;

	if (!pushConstantExists(name))
		throw std::runtime_error("PushConstant does not exist!");

	const auto& pc = *mPushConstants.at(name);

	std::vector<uint8_t> copy(pc.getSize());
	std::memcpy(copy.data(), data, pc.getSize());

	mPushConstantUploads.emplace_back([this, name, copy](const VkCommandBuffer commandBuffer)
		{
			mPushConstants.at(name)->upload(commandBuffer, copy.data());
		});
}

void PipelineRegistry::uploadAllPushConstants(const VkCommandBuffer commandBuffer)
{
	for (const auto& upload : mPushConstantUploads)
	{
		upload(commandBuffer);
	}

	mPushConstantUploads.clear();
}

void PipelineRegistry::reconstruct()
{
	cleanup();

	for (auto& entry : mGraphicsEntries)
	{
		std::vector<DescriptorSetLayoutPtr> layouts;

		for (const auto& layoutName : entry.mDescriptorSetLayouts)
		{
			auto& layout = DescriptorRegistryBackend::getDescriptorSetLayout(mDescriptorRegistry, layoutName);
			layouts.push_back(layout);
		}

		auto& vertex = VertexRegistryBackend::getVertex(mVertexRegistry, entry.mVertex);

		mGraphicsPipelines[entry.mName] = mGraphicsPipelineFactory.create(
			entry.mParams, entry.mVertexShader, entry.mPixelShader, vertex, layouts, mSwapchain, mRenderPass);

		const auto& pc = entry.mParams.mPushConstantRange;

		if (!pc.mName.empty() && pc.mSize > 0)
		{
			std::string pushConstantName = entry.mName + "_" + pc.mName;
			const auto& layout = mGraphicsPipelines.at(entry.mName)->getLayout();

			mPushConstants[pushConstantName] = mPushConstantFactory.create(
				layout,
				pc.mOffset,
				pc.mSize,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
		}
	}

	for (auto& entry : mComputeEntries)
	{
		std::vector<DescriptorSetLayoutPtr> layouts;

		for (const auto& layoutName : entry.mDescriptorSetLayouts)
		{
			auto& layout = DescriptorRegistryBackend::getDescriptorSetLayout(mDescriptorRegistry, layoutName);
			layouts.push_back(layout);
		}

		mComputePipelines[entry.mName] = mComputePipelineFactory.create(entry.mParams, entry.mComputeShader, layouts);

		const auto& pc = entry.mParams.mPushConstantRange;

		if (!pc.mName.empty() && pc.mSize > 0)
		{
			std::string pushConstantName = entry.mName + "_" + pc.mName;
			const auto& layout = mComputePipelines.at(entry.mName)->getLayout();

			mPushConstants[pushConstantName] = mPushConstantFactory.create(
				layout,
				pc.mOffset,
				pc.mSize,
				VK_SHADER_STAGE_COMPUTE_BIT);
		}
	}
}

void PipelineRegistry::cleanup()
{
	// push constants are not created and allocated like other resources.
	// They have no handle and do not need to be destroyed.
	mPushConstants.clear();

	for (auto& [name, pipeline] : mGraphicsPipelines)
	{
		if (pipeline) pipeline->destroy(mDevice);
	}
	mGraphicsPipelines.clear();

	for (auto& [name, pipeline] : mComputePipelines)
	{
		if (pipeline) pipeline->destroy(mDevice);
	}
	mComputePipelines.clear();
}

bool PipelineRegistry::isRegistered(const std::string& name) const
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

bool PipelineRegistry::pushConstantExists(const std::string& name) const
{
	return (mPushConstants.contains(name));
}

bool PipelineRegistry::graphicsPipelineExists(const std::string& name) const
{
	return (mGraphicsPipelines.contains(name));
}

bool PipelineRegistry::computePipelineExists(const std::string& name) const
{
	return (mComputePipelines.contains(name));
}
