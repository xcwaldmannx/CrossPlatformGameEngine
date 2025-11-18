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

#include "RenderGraph/RenderGraph.h"

#include "Resource/Barrier/Barrier.h"

#include "CommandPool/CommandPoolFactory.h"
#include "Descriptor/DescriptorFactory.h"
#include "Swapchain/SwapchainFactory.h"
#include "RenderPass/RenderPassFactory.h"
#include "Pipeline/GraphicsPipeline/GraphicsPipelineFactory.h"
#include "Pipeline/ComputePipeline/ComputePipelineFactory.h"
#include "Resource/Buffer/BufferFactory.h"
#include "Resource/Texture/TextureFactory.h"
#include "Resource/Sampler/SamplerFactory.h"

#include "../EcsSystem/EcsSystem.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Components/ModelComponent.h"
#include "Ecs/Systems/RenderSystem.h"

#include <memory>

namespace ascen
{

	class Engine
	{
	public:
		Engine(WindowManager& windowManager);

		const CommandPoolFactory& commandPool();
		const DescriptorFactory& descriptor();
		const SwapchainFactory& swapchain();
		const RenderPassFactory& renderPass();
		const GraphicsPipelineFactory& graphicsPipeline();
		const ComputePipelineFactory& computePipeline();
		const BufferFactory& buffer();
		const TextureFactory& texture();
		const SamplerFactory& sampler();

		EcsSystem& ecs();

		template<std::derived_from<Handle_I> T>
		void destroy(T& handle)
		{
			handle.destroy(mDevice);
		}

		template<typename T>
		void updateBuffer(const BufferPtr& buffer, const CommandPoolPtr& commandPool, const std::vector<T>& data)
		{
			buffer->update<T>(mPhysicalDevice, mDevice, mGraphicsFamily.value(), commandPool, data);
		}

		template<typename T>
		void updateUniformBuffer(const BufferPtr& buffer, const T& data)
		{
			size_t offset = mCurrentFrame * sizeof(T);
			uint8_t* target = reinterpret_cast<uint8_t*>(buffer->getMappedMemory());
			memcpy(target + offset, &data, sizeof(T));
		}

		void resize(
			const CommandPoolPtr& commandPool,
			const RenderPassPtr renderPass,
			SwapchainPtr& swapchain,
			TexturePtr& depthTexture);

		void drawFrame(
			const CommandPoolPtr& commandPool,
			const RenderPassPtr& renderPass,
			const SwapchainPtr& swapchain,
			RenderGraph& renderGraph,
			const BufferPtr& indirectBuffer,
			const std::vector<IndirectBuffer::DrawCommand> drawCommands,
			bool& isResized);

		uint32_t getGraphicsFamily() const;

		uint32_t getPresentFamily() const;

		void wait();

		void cleanup();

	private:
		void createSyncObjects();
		void destroySyncObjects();

	private:
		WindowManager& mWindowManager;

		std::vector<const char*> mExtensions;
		std::vector<const char*> mValidationLayers;
		VkDebugUtilsMessengerEXT mDebugMessenger{};

		VkInstance mInstance = VK_NULL_HANDLE;
		VkSurfaceKHR mSurface = VK_NULL_HANDLE;
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		QueueFamily mGraphicsFamily{};
		QueueFamily mPresentFamily{};
		VkDevice mDevice = VK_NULL_HANDLE;

		std::vector<VkSemaphore> mImageAvailableSemaphores;
		std::vector<VkSemaphore> mRenderFinishedSemaphores;
		std::vector<VkFence> mInFlightFences;
		
		uint32_t MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t mCurrentFrame = 0;
		uint32_t mCurrentImage = 0;

		// factories
		CommandPoolFactory mCommandPoolFactory;
		DescriptorFactory mDescriptorFactory;
		SwapchainFactory mSwapchainFactory;
		RenderPassFactory mRenderPassFactory;
		GraphicsPipelineFactory mGraphicsPipelineFactory;
		ComputePipelineFactory mComputePipelineFactory;
		BufferFactory mBufferFactory;
		TextureFactory mTextureFactory;
		SamplerFactory mSamplerFactory;

		EcsSystem mEcs;
	};

}