#pragma once

#include "../FrameGraph/FrameGraph.h"
#include "../Registry2/RegistryManager.h"

#include <vulkan/vulkan.h>

#include "../CommandRecorder/ComputeCommandRecorder/ComputeCommandRecorder.h"
#include "../CommandRecorder/LineCommandRecorder/LineCommandRecorder.h"
#include "../CommandRecorder/MeshCommandRecorder/MeshCommandRecorder.h"

class EcsSystem;

namespace ascen
{

	class WindowManager;
	class VulkanContext;
	class RenderContext;
	class ResourceRegistry;
	class DescriptorRegistry;
	class FramePassRegistry;
	class RenderTargetRegistry;

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
			RegistryManager& registryManager);

		// void updateRenderSystem();

		void drawFrame();

		// void updateModels(std::unordered_map<uint32_t, ModelData>& modelData);

		void createSyncObjects();
		void destroySyncObjects();

		void cleanup();

		uint32_t getFrameIndex() const;

	private:
		void acquireNextFrame(const SwapchainPtr& swapchain);
		void submitFrame(const CommandPoolPtr& commandPool) const;
		void presentFrame(const SwapchainPtr& swapchain) const;

	private:
		WindowManager& mWindowManager;
		EcsSystem& mEcsSystem;
		const VkPhysicalDevice mPhysicalDevice;
		const VkQueue mGraphicsQueue;
		const VkQueue mPresentQueue;
		const VkDevice mDevice;
		RenderContext& mRenderContext;
		RegistryManager& mRegistryManager;

		FrameGraph mFrameGraph;

		LineCommandRecorder mLineCommandRecorder;
		MeshCommandRecorder mMeshCommandRecorder;
		ComputeCommandRecorder mComputeCommandRecorder;

		std::vector<VkSemaphore> mImageAvailableSemaphores;
		std::vector<VkSemaphore> mRenderFinishedForImageSemaphores;
		std::vector<VkFence> mInFlightFences;


		uint32_t MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t mFrameIndex = 0;
		uint32_t mImageIndex = 0;

		uint32_t mDrawCommandCount = 0;
	};

}
