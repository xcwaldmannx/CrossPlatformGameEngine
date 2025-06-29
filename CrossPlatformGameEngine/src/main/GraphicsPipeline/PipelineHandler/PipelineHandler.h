#pragma once

#include "../LogicalDeviceHandler/LogicalDeviceHandler.h"
#include "../SwapchainHandler/SwapchainHandler.h"
#include "../RenderPassHandler/RenderPassHandler.h"
#include "../ShaderHandler/ShaderHandler.h"

#ifndef INCLUDE_DESCRIPTORSET_HANDLER_H
#define INCLUDE_DESCRIPTORSET_HANDLER_H
#include "../DescriptorSetHandler/DescriptorSetHandler.h"
#endif

#include <vulkan/vulkan.h>


namespace ascen {

	struct DescriptorGroup;
	struct RenderPass;

	struct Pipeline {
		VkPipeline mPipeline;
		VkPipelineLayout mLayout;
	};

	/*
	FILE: PipelineHandler.h
	NAME: createPipeline
	RTRN: void
	DESC: Handles Pipeline creation
	*/
	void createPipeline(
		LogicalDevice& logicalDevice,
		Swapchain& swapchain,
		ShaderModule& vertShader,
		ShaderModule& fragShader,
		DescriptorGroup& descriptorGroup,
		RenderPass& renderpass,
		Pipeline* pipeline);

	/*
	FILE: PipelineHandler.h
	NAME: destroyPipeline
	RTRN: void
	DESC: Handles Pipeline destruction
	*/
	void destroyPipeline(LogicalDevice& logicalDevice, Pipeline& pipeline);
}
