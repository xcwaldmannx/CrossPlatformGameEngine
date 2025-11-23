#include "PipelineRegistry.h"

#include "../Vertex/VertexRegistryBackend.h"
#include "../Descriptor/DescriptorRegistryBackend.h"

using namespace ascen;

PipelineRegistry::PipelineRegistry(
	const VulkanContext& vulkanContext,
	const RenderContext& renderContext,
	const VertexRegistry& vertexRegistry,
	const DescriptorRegistry& descriptorRegistry) :
	mDevice(vulkanContext.getDevice()),
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
		throw std::runtime_error("A pipeline with that name already exists!");
	}

	mRegisteredNames.push_back(entry.mName);
	mGraphicsEntries.emplace_back(std::move(entry));
}

void PipelineRegistry::registerComputePipeline(ComputePipelineEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A pipeline with that name already exists!");
	}

	mRegisteredNames.push_back(entry.mName);
	mComputeEntries.emplace_back(std::move(entry));
}

void PipelineRegistry::reconstruct()
{
	cleanup();

	for (auto& entry : mGraphicsEntries)
	{
		std::vector<DescriptorSetLayoutPtr> layouts;

		auto& engineLayout = DescriptorRegistryBackend::getDescriptorSetLayout(mDescriptorRegistry, "engine");
		layouts.push_back(engineLayout);

		for (const auto& layoutName : entry.mDescriptorSetLayouts)
		{
			auto& layout = DescriptorRegistryBackend::getDescriptorSetLayout(mDescriptorRegistry, layoutName);
			layouts.push_back(layout);
		}

		auto& vertex = VertexRegistryBackend::getVertex(mVertexRegistry, entry.mVertex);

		mGraphicsPipelines[entry.mName] = mGraphicsPipelineFactory.create(
			entry.mVertexShader, entry.mPixelShader, vertex, layouts, mSwapchain, mRenderPass);
	}

	for (auto& entry : mComputeEntries)
	{
		std::vector<DescriptorSetLayoutPtr> layouts;

		for (const auto& layoutName : entry.mDescriptorSetLayouts)
		{
			auto& layout = DescriptorRegistryBackend::getDescriptorSetLayout(mDescriptorRegistry, layoutName);
			layouts.push_back(layout);
		}

		mComputePipelines[entry.mName] = mComputePipelineFactory.create(entry.mComputeShader, layouts);
	}
}

void PipelineRegistry::cleanup()
{
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

bool PipelineRegistry::graphicsPipelineExists(const std::string& name) const
{
	return (mGraphicsPipelines.find(name) != mGraphicsPipelines.end());
}

bool PipelineRegistry::computePipelineExists(const std::string& name) const
{
	return (mComputePipelines.find(name) != mComputePipelines.end());
}
