#include "WireframePipeline.h"

WireframePipeline::WireframePipeline(
	const std::string vertexShaderFilepath,
	const std::string pixelShaderFilepath,
	const VkExtent2D& swapchainExtent,
	VkDescriptorSetLayout descriptorSetLayout,
	VkRenderPass renderPass)
	: Pipeline(vertexShaderFilepath,
		pixelShaderFilepath,
		swapchainExtent,
		descriptorSetLayout,
		renderPass)
{
	mPipelineInfo.mRasterizationStateInfo.polygonMode = VK_POLYGON_MODE_LINE;
}