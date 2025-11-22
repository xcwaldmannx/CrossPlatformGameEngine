#include "GraphicsPipelineFactory.h"

using namespace ascen;

GraphicsPipelineFactory::GraphicsPipelineFactory(VkDevice device) : mDevice(device) {}

GraphicsPipelinePtr GraphicsPipelineFactory::create(
	const std::string& vertexShaderFilepath,
	const std::string& pixelShaderFilepath,
	const VertexPtr& vertex,
	const std::vector<DescriptorSetLayoutPtr>& descriptorSetLayouts,
	const SwapchainPtr& swapchain,
	const RenderPassPtr& renderPass) const
{
	GraphicsPipelinePtr ptr(new GraphicsPipeline(
		mDevice,
		vertexShaderFilepath,
		pixelShaderFilepath,
		vertex,
		descriptorSetLayouts,
		swapchain,
		renderPass));
	return ptr;
}
