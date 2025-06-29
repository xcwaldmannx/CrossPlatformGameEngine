#pragma once

#include "Support/ValidationLayerSupport/ValidationLayerManager.h"
#include "Support/ExtensionSupport/ExtensionManager.h"
#include "Support/DebugSupport/DebugManager.h"

#include "QueueFamily/QueueFamilyManager.h"

#include "Window/WindowManager.h"

#include "Surface/SurfaceManager.h"
#include "LogicalDevice/LogicalDeviceManager.h"
#include "PhysicalDevice/PhysicalDeviceManager.h"

#include "Swapchain/SwapchainManager.h"
#include "RenderPass/RenderPassManager.h"
#include "GraphicsPipeline/GraphicsPipelineManager.h"
#include "Command/CommandManager.h"

#include "Shader/ShaderManager.h"

#include "ResourceManager/ResourceManager.h"

#include "Vertex/Vertex.h"

#include "ResourceManager/Mesh/Mesh.h"

#include "Descriptor/DescriptorSetLayout.h"


#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <optional>

class VulkanManager {
public:
	VulkanManager();
	void init();
	void mainLoop();
	void destroy();

private:
	void createInstance();

	void createSyncObjects();
	void drawFrame();

private:
	// WindowManager BEGIN

	vulkan::WindowManager mWindowManager;

	// WindowManager END

	const int MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t mCurrentFrame = 0;

	VkInstance mVkInstance = nullptr;

	// ValidationLayerManager BEGIN
	
	vulkan::ValidationLayerManager mValidationLayerManager;
	
	// ValidationLayerManager END

	// ExtensionManager BEGIN

	vulkan::ExtensionManager mExtensionManager;

	// ExtensionManager END

	// DebugManager BEGIN

	vulkan::DebugManager mDebugManager;

	// DebugManager END

	// QueueFamilyManager BEGIN

	vulkan::QueueFamilyManager mQueueFamilyManager;

	// QueueFamilyManager END

	// SurfaceManager BEGIN

	vulkan::SurfaceManager mSurfaceManager;

	// SurfaceManager END

	// PhysicalDeviceManager BEGIN

	vulkan::PhysicalDeviceManager mPhysicalDeviceManager;

	// PhysicalDeviceManager END

	// LogicalDeviceManager BEGIN
	
	vulkan::LogicalDeviceManager mLogicalDeviceManager;

	// LogicalDeviceManager END

	// SwapchainManager BEGIN

	vulkan::SwapchainManager mSwapchainManager;

	// SwapchainManager END

	// RenderPassManager BEGIN

	vulkan::RenderPassManager mRenderPassManager;

	// RenderPassManager END

	// GraphicsPipelineManager BEGIN

	vulkan::GraphicsPipelineManager mGraphicsPipelineManager;

	// GraphicsPipelineManager END

	// ShaderManager BEGIN

	vulkan::ShaderManager mShaderManager;

	// ShaderManager END

	// CommandManager BEGIN
	
	vulkan::CommandManager mCommandManager;

	// CommandManager END

	std::vector<VkSemaphore> mImageAvailableSemaphores;
	std::vector<VkSemaphore> mRenderFinishedSemaphores;
	std::vector<VkFence> mInFlightFences;

	// ResourceManager BEGIN

	vulkan::ResourceManager mResourceManager;

	vulkan::Buffer mVertexBuffer;
	vulkan::Buffer mIndexBuffer;
	vulkan::Mesh mMesh;

	// ResourceManager END

	// Descriptor Sets BEGIN

	vulkan::DescriptorSetLayout mDescriptorSetLayout;

	// Descriptor Sets END
};