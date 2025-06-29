#pragma once

#include <vulkan/vulkan.h>


namespace vulkan {
	class SwapchainManager;

	class RenderPassManager {
	public:
		VkResult create(const SwapchainManager& swapchain, const VkDevice& logicalDevice);
		void destroy(const VkDevice& logicalDevice);

		VkRenderPass getRenderPass() const;

	private:
		VkRenderPass mVkRenderPass = nullptr;

		friend class GraphicsPipelineManager;
		friend class SwapchainManager;
	};

}