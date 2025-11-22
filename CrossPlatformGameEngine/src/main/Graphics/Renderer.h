#pragma once

#include "../WindowManager/WindowManager.h"
#include "Engine.h"
#include "RenderGraph/RenderGraph.h"

#include <concepts>

#include <unordered_map>

#include <vulkan/vulkan.h>

namespace ascen
{

	class Renderer
	{
	public:

		struct Pass
		{
			std::vector<std::string> mVertexBuffers;
			std::string mIndexBuffer;
			std::vector<std::string> mReadBuffers;
			std::vector<std::string> mWriteBuffers;
			std::vector<std::string> mReadTextures;
			std::vector<std::string> mWriteTextures;
			std::vector<std::string> mDescriptorSets;
			std::string mGraphicsPipeline;
			std::string mComputePipeline;
		};

		Renderer(
			WindowManager& windowManager,
			EcsSystem& ecsSystem,
			VulkanContext& vulkanContext,
			RenderContext& renderContext,
			ResourceRegistry& resourceRegistry,
			DescriptorRegistry& descriptorRegistry);

		void updateRenderSystem();

		void drawFrame();

		void updateModels(std::unordered_map<uint32_t, ModelData>& modelData);

		void createSyncObjects();
		void destroySyncObjects();

		void cleanup();

		void createRenderPass(const std::string& name, const Pass& renderPass);

	private:
		WindowManager& mWindowManager;
		EcsSystem& mEcsSystem;
		const VkPhysicalDevice mPhysicalDevice;
		const VkQueue mGraphicsQueue;
		const VkQueue mPresentQueue;
		const VkDevice mDevice;
		RenderContext& mRenderContext;
		ResourceRegistry& mResourceRegistry;
		DescriptorRegistry& mDescriptorRegistry;

		RenderGraph mRenderGraph;

		std::vector<VkSemaphore> mImageAvailableSemaphores;
		std::vector<VkSemaphore> mRenderFinishedSemaphores;
		std::vector<VkFence> mInFlightFences;

		uint32_t MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t mCurrentFrame = 0;
		uint32_t mCurrentImage = 0;

		BufferPtr mInstanceBuffer = nullptr;
		BufferPtr mIndirectBuffer = nullptr;

		uint32_t mDrawCommandCount = 0;
	};

}
