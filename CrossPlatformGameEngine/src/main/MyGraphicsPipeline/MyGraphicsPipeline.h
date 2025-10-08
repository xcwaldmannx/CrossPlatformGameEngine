#pragma once

#include "../WindowManager/WindowManager.h"
#include "../Graphics/QueueFamilies/QueueFamilies.h"

#include "../Graphics/HandleManager/HandleManager.h"
#include "../Graphics/Swapchain/Swapchain.h"
#include "../Graphics/RenderPass/RenderPass.h"
#include "../Graphics/CommandPool/CommandPool.h"

#include "../Graphics/Resource/Buffer/Buffer.h"

#include "../Graphics/Pipeline/WireframePipeline.h"

#include <memory>

#include <stack>
#include <vector>

#include <vulkan/vulkan.h>

class MyGraphicsPipeline
{
public:
	MyGraphicsPipeline();

	void create();
	void destroy();

private:
	void createDepthTexture();
	void createBuffers();

private:
	WindowManager mWindowManager;

	std::vector<const char*> mExtensions;
	std::vector<const char*> mValidationLayers;
	VkDebugUtilsMessengerEXT mDebugMessenger;

	VkInstance mInstance;
	VkSurfaceKHR mSurface;

	VkPhysicalDevice mPhysicalDevice;
	ascen::QueueFamily mGraphicsFamily;
	ascen::QueueFamily mPresentFamily;

	VkDevice mDevice;

	ascen::HandleManager mHandleManager;

	VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet mDescriptorSet = VK_NULL_HANDLE;

	std::shared_ptr<ascen::Swapchain> mSwapchain = nullptr;
	std::shared_ptr<ascen::RenderPass> mRenderPass = nullptr;
	std::shared_ptr<ascen::Pipeline> mPipeline = nullptr;
	std::shared_ptr<ascen::CommandPool> mCommandPool = nullptr;

	std::shared_ptr<ascen::Buffer> mUniformBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mVertexBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mIndexBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mStorageBuffer = nullptr;
};