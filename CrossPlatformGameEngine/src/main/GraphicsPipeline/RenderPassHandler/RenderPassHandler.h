#pragma once

#include "../LogicalDeviceHandler/LogicalDeviceHandler.h"

#ifndef INCLUDE_SWAPCHAIN_HANDLER_H
#include "../SwapchainHandler/SwapchainHandler.h"
#define INCLUDE_SWAPCHAIN_HANDLER_H
#endif

#include <vulkan/vulkan.h>

namespace ascen {

	struct Swapchain;

	struct RenderPass {
		VkRenderPass mRenderPass;
	};

	/*
	FILE: RenderPassHandler.h
	NAME: createRenderPass
	RTRN: void
	DESC: Handles render pass creation
	*/
	void createRenderPass(
		PhysicalDevice& physicalDevice, 
		LogicalDevice& logicalDevice,
		Swapchain& swapchain,
		RenderPass& renderpass);

	/*
	FILE: RenderPassHandler.h
	NAME: destroyRenderPass
	RTRN: void
	DESC: Handles render pass destruction
	*/
	void destroyRenderPass(LogicalDevice& logicalDevice, RenderPass& renderpass);

}