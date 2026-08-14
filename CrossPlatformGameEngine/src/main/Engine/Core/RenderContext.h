#pragma once

#include "../../WindowManager/WindowManager.h"
#include "VulkanContext.h"

#include "Types.h"

namespace ascen
{

	class CommandPoolFactory;
	class SwapchainFactory;
	class RenderPassFactory;
	class TextureFactory;

	class RenderContext
	{
	public:
		RenderContext(
			WindowManager& windowManager,
			const VulkanContext& vulkanContext);

		void resize();

		void cleanup() const;

		const CommandPoolPtr& getCommandPool() const;
		const SwapchainPtr& getSwapchain() const;

	private:
		WindowManager& mWindowManager;
		const VkDevice mDevice;
		const uint32_t mGraphicsFamilyIndex;
		const uint32_t mPresentFamilyIndex;
		const CommandPoolFactory& mCommandPoolFactory;
		const SwapchainFactory& mSwapchainFactory;
		const RenderPassFactory& mRenderPassFactory;
		const TextureFactory& mTextureFactory;

		CommandPoolPtr mCommandPool = nullptr;
		SwapchainPtr mSwapchain     = nullptr;
		TexturePtr mDepthTexture    = nullptr;
	};

}
