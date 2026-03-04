#include "GraphicsPipelineFactory.h"

#include "GraphicsPipeline.h"

using namespace ascen;

GraphicsPipelineFactory::GraphicsPipelineFactory(VkDevice device) : mDevice(device) {}

GraphicsPipelinePtr GraphicsPipelineFactory::create(
	const GraphicsPipelineParams& params,
	const std::string& vertexShaderFilepath,
	const std::string& pixelShaderFilepath,
	const VertexPtr& vertex,
	const std::vector<DescriptorSetLayoutPtr>& descriptorSetLayouts,
	const SwapchainPtr& swapchain,
	const RenderPassPtr& renderPass) const
{
	GraphicsPipelinePtr ptr(new GraphicsPipeline(
		mDevice,
		params,
		vertexShaderFilepath,
		pixelShaderFilepath,
		vertex,
		descriptorSetLayouts,
		swapchain,
		renderPass));
	return ptr;
}
