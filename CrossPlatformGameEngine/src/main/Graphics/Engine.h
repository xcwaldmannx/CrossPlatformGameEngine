#pragma once

#include "../WindowManager/WindowManager.h"

#include "Extensions/Extensions.h"
#include "ValidationLayers/ValidationLayers.h"
#include "Instance/Instance.h"
#include "Surface/Surface.h"
#include "DebugMessenger/DebugMessenger.h"

#include "Device/Physical/PhysicalDevice.h"
#include "Device/Logical/Device.h"
#include "QueueFamilies/QueueFamilies.h"

#include "Resource/Buffer/BufferFactory.h"

#include "Resource/Barrier/Barrier.h"
#include "Resource/Image/Image.h"
#include "Resource/Texture/Texture.h"
#include "Resource/Sampler/Sampler.h"

#include "CommandPool/CommandPoolFactory.h"
#include "Descriptor/DescriptorFactory.h"
#include "Swapchain/SwapchainFactory.h"
#include "RenderPass/RenderPassFactory.h"
#include "Pipeline/GraphicsPipeline/GraphicsPipelineFactory.h"
#include "Pipeline/ComputePipeline/ComputePipelineFactory.h"

#include <memory>

namespace ascen
{

	class Engine
	{
	public:
		Engine(WindowManager* windowManager);

		const CommandPoolFactory& commandPool();
		const DescriptorFactory& descriptor();
		const SwapchainFactory& swapchain();
		const RenderPassFactory& renderPass();
		const GraphicsPipelineFactory& graphicsPipeline();
		const ComputePipelineFactory& computePipeline();
		const BufferFactory& buffer();

		template<std::derived_from<Handle_I> T>
		void destroy(T& handle)
		{
			handle.destroy(mDevice);
		}

		void destroyBuffer(const std::shared_ptr<Buffer>& buffer) const;

		void resize(
			const CommandPoolPtr& commandPool,
			const RenderPassPtr renderPass,
			SwapchainPtr& swapchain,
			std::shared_ptr<Texture>& depthTexture);

		uint32_t getGraphicsFamily() const;

		uint32_t getPresentFamily() const;

		// TODO: replace below with factories

		std::shared_ptr<Texture> createTexture(
			const std::shared_ptr<CommandPool>& commandPool,
			const std::vector<unsigned char>& pixels,
			uint32_t width,
			uint32_t height,
			uint32_t layers);
		void destroyTexture(const std::shared_ptr<Texture>& texture) const;

		std::shared_ptr<Sampler> createSampler();
		void destroySampler(const std::shared_ptr<Sampler>& sampler) const;

	private:
		WindowManager* mWindowManager = nullptr;

		std::vector<const char*> mExtensions;
		std::vector<const char*> mValidationLayers;
		VkDebugUtilsMessengerEXT mDebugMessenger{};

		VkInstance mInstance = VK_NULL_HANDLE;
		VkSurfaceKHR mSurface = VK_NULL_HANDLE;
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		QueueFamily mGraphicsFamily{};
		QueueFamily mPresentFamily{};
		VkDevice mDevice = VK_NULL_HANDLE;

		// factories
		CommandPoolFactory mCommandPoolFactory;
		DescriptorFactory mDescriptorFactory;
		SwapchainFactory mSwapchainFactory;
		RenderPassFactory mRenderPassFactory;
		GraphicsPipelineFactory mGraphicsPipelineFactory;
		ComputePipelineFactory mComputePipelineFactory;
		BufferFactory mBufferFactory;
	};

}