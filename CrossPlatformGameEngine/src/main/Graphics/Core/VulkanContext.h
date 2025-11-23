#pragma once

#include "../CommandPool/CommandPoolFactory.h"
#include "../Descriptor/DescriptorFactory.h"
#include "../Swapchain/SwapchainFactory.h"
#include "../RenderPass/RenderPassFactory.h"
#include "../Pipeline/GraphicsPipeline/GraphicsPipelineFactory.h"
#include "../Pipeline/ComputePipeline/ComputePipelineFactory.h"
#include "../Resource/Buffer/BufferFactory.h"
#include "../Resource/Texture/TextureFactory.h"
#include "../Resource/Sampler/SamplerFactory.h"

#include <vulkan/vulkan.h>

class WindowManager;

namespace ascen
{

	using ::WindowManager;

	class VulkanContext
	{
	public:
		VulkanContext(WindowManager& windowManager);

		VkPhysicalDevice getPhysicalDevice() const;
		VkDevice getDevice() const;
		uint32_t getGraphicsFamilyIndex() const;
		uint32_t getPresentFamilyIndex() const;
		VkQueue getGraphicsQueue() const;
		VkQueue getPresentQueue() const;

		const CommandPoolFactory& getCommandPoolFactory() const;
		const DescriptorFactory& getDescriptorFactory() const;
		const SwapchainFactory& getSwapchainFactory() const;
		const RenderPassFactory& getRenderPassFactory() const;
		const GraphicsPipelineFactory& getGraphicsPipelineFactory() const;
		const ComputePipelineFactory& getComputePipelineFactory() const;
		const BufferFactory& getBufferFactory() const;
		const TextureFactory& getTextureFactory() const;
		const SamplerFactory& getSamplerFactory() const;

		void waitIdle() const;

		void cleanup() const;

	private:
		WindowManager& mWindowManager;

		std::vector<const char*> mExtensions;
		std::vector<const char*> mValidationLayers;
		VkDebugUtilsMessengerEXT mDebugMessenger{};

		VkInstance mInstance = VK_NULL_HANDLE;
		VkSurfaceKHR mSurface = VK_NULL_HANDLE;
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		uint32_t mGraphicsFamilyIndex = 0;
		uint32_t mPresentFamilyIndex = 0;
		VkQueue mGraphicsQueue = VK_NULL_HANDLE;
		VkQueue mPresentQueue = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;

		CommandPoolFactory mCommandPoolFactory;
		DescriptorFactory mDescriptorFactory;
		SwapchainFactory mSwapchainFactory;
		RenderPassFactory mRenderPassFactory;
		GraphicsPipelineFactory mGraphicsPipelineFactory;
		ComputePipelineFactory mComputePipelineFactory;
		BufferFactory mBufferFactory;
		TextureFactory mTextureFactory;
		SamplerFactory mSamplerFactory;
	};

}
