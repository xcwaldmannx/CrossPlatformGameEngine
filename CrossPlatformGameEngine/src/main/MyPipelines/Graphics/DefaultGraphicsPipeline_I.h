//#pragma once
//
//#include "../../WindowManager/WindowManager.h"
//
//#include "../../Graphics/Extensions/Extensions.h"
//#include "../../Graphics/ValidationLayers/ValidationLayers.h"
//#include "../../Graphics/Instance/Instance.h"
//#include "../../Graphics/Surface/Surface.h"
//#include "../../Graphics/DebugMessenger/DebugMessenger.h"
//#include "../../Graphics/Device/Physical/PhysicalDevice.h"
//#include "../../Graphics/Device/Logical/Device.h"
//#include "../../Graphics/Descriptor/Layout/DescriptorSetLayout.h"
//#include "../../Graphics/QueueFamilies/QueueFamilies.h"
//#include "../../Graphics/Swapchain/Swapchain.h"
//#include "../../Graphics/RenderPass/RenderPass.h"
//#include "../../Graphics/CommandPool/CommandPool.h"
//#include "../../Graphics/Pipeline/GraphicsPipeline/GraphicsPipeline.h"
//#include "../../Graphics/Pipeline/ComputePipeline/ComputePipeline.h"
//#include "../../Graphics/Vertex/Vertex_I.h"
//#include "../../Graphics/Resource/Buffer/Buffer.h"
//#include "../../Graphics/Resource/Image/Image.h"
//#include "../../Graphics/Resource/Texture/Texture.h"
//#include "../../Graphics/Resource/Sampler/Sampler.h"
//
//#include <concepts>
//#include <cstdint>
//#include <memory>
//
//#include <vector>
//
//#include <vulkan/vulkan.h>
//
//
//class DefaultGraphicsPipeline_I
//{
//public:
//	DefaultGraphicsPipeline_I(
//		WindowManager* windowManager,
//		const std::string& vertexShaderFilepath,
//		const std::string& pixelShaderFilepath) :
//		mWindowManager(windowManager),
//		mVertexShaderFilepath(vertexShaderFilepath),
//		mPixelShaderFilepath(pixelShaderFilepath){}
//
//	virtual void create() = 0;
//	virtual void destroy() = 0;
//
//	virtual void submit(std::vector<VkDrawIndexedIndirectCommand> drawCommands) = 0;
//
//	virtual void record(uint32_t currentImage) = 0;
//
//	virtual void drawFrame() = 0;
//
//	virtual void resize() = 0;
//
//	virtual bool isResized() const = 0;
//
//protected:
//	virtual void createShaderResources() = 0;
//	virtual void destroyShaderResources() = 0;
//
//	virtual void createDescriptorResources() = 0;
//	virtual void destroyDescriptorResources() = 0;
//
//	virtual void createSyncObjects() = 0;
//	virtual void destroySyncObjects() = 0;
//
//protected:
//	WindowManager* mWindowManager = nullptr;
//	bool mIsWindowResized = false;
//
//	std::vector<const char*> mExtensions;
//	std::vector<const char*> mValidationLayers;
//	VkDebugUtilsMessengerEXT mDebugMessenger{};
//
//	VkInstance mInstance = VK_NULL_HANDLE;
//	VkSurfaceKHR mSurface = VK_NULL_HANDLE;
//
//	VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
//	ascen::QueueFamily mGraphicsFamily;
//	ascen::QueueFamily mPresentFamily;
//
//	VkDevice mDevice = VK_NULL_HANDLE;
//
//	std::shared_ptr<ascen::DescriptorSetLayout> mDescriptor = nullptr;
//
//	const std::string mVertexShaderFilepath;
//	const std::string mPixelShaderFilepath;
//
//	std::shared_ptr<ascen::Swapchain> mSwapchain = nullptr;
//	std::shared_ptr<ascen::RenderPass> mRenderPass = nullptr;
//	std::shared_ptr<ascen::GraphicsPipeline_I> mPipeline = nullptr;
//	std::shared_ptr<ascen::CommandPool> mCommandPool = nullptr;
//
//	std::shared_ptr<ascen::Texture> mDepthTexture = nullptr;
//
//	std::vector<VkSemaphore> mImageAvailableSemaphores;
//	std::vector<VkSemaphore> mRenderFinishedSemaphores;
//	std::vector<VkFence> mInFlightFences;
//
//	uint32_t MAX_FRAMES_IN_FLIGHT = 2;
//	uint32_t mCurrentFrame = 0;
//	uint32_t mCurrentImage = 0;
//
//	std::vector<VkDrawIndexedIndirectCommand> mDrawCommands;
//};
