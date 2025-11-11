#pragma once

#include "GraphicsPipeline.h"

#include <concepts>

#include <string>

template<std::derived_from<ascen::Vertex_I> T>
class WireframePipeline : public ascen::Pipeline<T>
{
public:
	WireframePipeline(
		const std::string vertexShaderFilepath,
		const std::string pixelShaderFilepath,
		const VkExtent2D& swapchainExtent,
		VkDescriptorSetLayout descriptorSetLayout,
		VkRenderPass renderPass)
		: ascen::Pipeline<T>(vertexShaderFilepath,
			pixelShaderFilepath,
			swapchainExtent,
			descriptorSetLayout,
			renderPass)
	{
		this->mRasterizationStateInfo.polygonMode = VK_POLYGON_MODE_LINE;
		this->mRasterizationStateInfo.cullMode = VK_CULL_MODE_NONE;
	}
};