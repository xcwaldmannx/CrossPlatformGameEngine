#pragma once

#include "../WindowManager/WindowManager.h"
#include "VulkanContext.h"

namespace ascen
{

	class RenderContext
	{
	public:
		RenderContext(
			WindowManager& windowManager,
			VulkanContext& context);

		void resize(
			const CommandPoolPtr& commandPool,
			const RenderPassPtr renderPass,
			SwapchainPtr& swapchain,
			TexturePtr& depthTexture);

		void cleanup();

		const CommandPoolPtr& getCommandPool();
		const SwapchainPtr& getSwapchain();
		const RenderPassPtr& getRenderPass();

	private:
		const WindowManager& mWindowManager;
		const VkDevice mDevice;
		const uint32_t mGraphicsFamilyIndex;
		const uint32_t mPresentFamilyIndex;
		const CommandPoolFactory& mCommandPoolFactory;
		const SwapchainFactory& mSwapchainFactory;
		const RenderPassFactory& mRenderPassFactory;
		const TextureFactory& mTextureFactory;

		CommandPoolPtr mCommandPool = nullptr;
		SwapchainPtr mSwapchain     = nullptr;
		RenderPassPtr mRenderPass   = nullptr;
		TexturePtr mDepthTexture    = nullptr;
	};

}
