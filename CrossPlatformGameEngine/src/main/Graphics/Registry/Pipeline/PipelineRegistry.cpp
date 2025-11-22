#include "PipelineRegistry.h"

#include "../Vertex/VertexRegistryBackend.h"
#include "../Descriptor/DescriptorRegistryBackend.h"

using namespace ascen;

PipelineRegistry::PipelineRegistry(
	VulkanContext& vulkanContext,
	RenderContext& renderContext,
	VertexRegistry& vertexRegistry,
	DescriptorRegistry& descriptorRegistry) :
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
		throw std::runtime_error("Pipeline name already registered!");
	}

	mRegisteredNames.push_back(entry.mName);
	mGraphicsEntries.emplace_back(std::move(entry));
}

void PipelineRegistry::registerComputePipeline(ComputePipelineEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("Pipeline name already registered!");
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

		for (const auto& layoutName : entry.mDescriptorSetLayouts)
		{
			auto& layout = DescriptorRegistryBackend::getDescriptorLayout(mDescriptorRegistry, layoutName);
			layouts.push_back(layout);
		}

		auto& vertex = VertexRegistryBackend::getVertex(mVertexRegistry, entry.mVertex);

		mGraphicsPipelineFactory.create(entry.mVertexShader, entry.mPixelShader, vertex, layouts, mSwapchain, mRenderPass);
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
