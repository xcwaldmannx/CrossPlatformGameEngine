#pragma once

#include "Pipeline.h"

#include <string>

class WireframePipeline : public ascen::Pipeline
{
public:
	WireframePipeline(
		const std::string vertexShaderFilepath,
		const std::string pixelShaderFilepath,
		const VkExtent2D& swapchainExtent,
		VkDescriptorSetLayout descriptorSetLayout,
		VkRenderPass renderPass);
};