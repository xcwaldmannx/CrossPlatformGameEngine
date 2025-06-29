#pragma once

#include "../SurfaceHandler/SurfaceHandler.h"
#include "../PhysicalDeviceHandler/PhysicalDeviceHandler.h"
#include "../LogicalDeviceHandler/LogicalDeviceHandler.h"

#ifndef INCLUDE_RENDERPASS_HANDLER_H
#include "../RenderPassHandler/RenderPassHandler.h"
#define INCLUDE_RENDERPASS_HANDLER_H
#endif

#include "../Resource/TextureHandler/TextureHandler.h"

#include <vector>

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <Vulkan/vulkan.h>


namespace ascen {

	struct RenderPass;

	struct Swapchain {
		VkSwapchainKHR mSwapchain = nullptr;
		std::vector<VkImage> mImages;
		VkFormat mImageFormat;
		VkExtent2D mExtent;
		std::vector<VkImageView> mImageViews;
		std::vector<VkFramebuffer> mFrameBuffers;
	};

	struct SwapchainSupportDetails {
		VkSurfaceCapabilitiesKHR mCapabilities;
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
	};

	/*
	FILE: SwapchainHandler.h
	NAME: createSwapchain
	RTRN: void
	DESC: Handles Swapchain creation
	*/
	void createSwapchain(
		GLFWwindow* window,
		PhysicalDevice& physicalDevice,
		LogicalDevice& logicalDevice,
		Surface& surface,
		uint32_t graphicsFamily,
		uint32_t presentFamily,
		Swapchain& swapchain);

	/*
	FILE: SwapchainHandler.h
	NAME: recreateSwapchain
	RTRN: void
	DESC: Handles Swapchain recreation
	*/
	void recreateSwapchain(
		GLFWwindow* window,
		PhysicalDevice& physicalDevice,
		LogicalDevice& logicalDevice,
		Surface& surface,
		uint32_t graphicsFamily,
		uint32_t presentFamily,
		Swapchain& swapchain);

	/*
	FILE: SwapchainHandler.h
	NAME: destroySwapchain
	RTRN: void
	DESC: Handles Swapchain destruction
	*/
	void destroySwapchain(LogicalDevice& logicalDevice, Swapchain& swapchain);

	/*
	FILE: SwapchainHandler.h
	NAME: createFrameBuffers
	RTRN: void
	DESC: Handles Frame Buffer creation
	*/
	void createFrameBuffers(
		LogicalDevice& logicalDevice,
		Swapchain& swapchain,
		RenderPass& renderpass,
		Texture& depthTexture);

	/*
	FILE: SwapchainHandler.h
	NAME: destroyFrameBuffers
	RTRN: void
	DESC: Handles Frame Buffer destruction
	*/
	void destroyFrameBuffers(LogicalDevice& logicalDevice, Swapchain& swapchain);

}