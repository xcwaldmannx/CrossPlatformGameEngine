#pragma once

#include "../WindowManager/WindowManager.h"

#include "VulkanContext.h"
#include "RenderContext.h"

#include "Registry/Vertex/VertexRegistry.h"
#include "Registry/Resource/ResourceRegistry.h"
#include "Registry/Descriptor/DescriptorRegistry.h"

#include "RenderGraph/RenderGraph.h"

#include "Resource/Barrier/Barrier.h"

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

		VertexRegistry& vertex();
		ResourceRegistry& resource();
		DescriptorRegistry& descriptor();

		EcsSystem& ecs();

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

		void drawFrame(
			const CommandPoolPtr& commandPool,
			const RenderPassPtr& renderPass,
			const SwapchainPtr& swapchain,
			RenderGraph& renderGraph,
			const BufferPtr& indirectBuffer,
			const std::vector<IndirectBuffer::DrawCommand> drawCommands,
			bool& isResized);

		void cleanup();

	private:
		void createSyncObjects();
		void destroySyncObjects();

	private:
		WindowManager& mWindowManager;

		VulkanContext mVulkanContext;
		RenderContext mRenderContext;

		VertexRegistry mVertexRegistry;
		ResourceRegistry mResourceRegistry;
		DescriptorRegistry mDescriptorRegistry;

		std::vector<VkSemaphore> mImageAvailableSemaphores;
		std::vector<VkSemaphore> mRenderFinishedSemaphores;
		std::vector<VkFence> mInFlightFences;
		
		uint32_t MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t mCurrentFrame = 0;
		uint32_t mCurrentImage = 0;

		EcsSystem mEcs;
	};

}