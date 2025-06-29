#pragma once

#include "../Swapchain/SwapchainManager.h"
#include "../RenderPass/RenderPassManager.h"
#include "../Shader/ShaderManager.h"
#include "../Descriptor/DescriptorSetLayout.h"

#include <vulkan/vulkan.h>

namespace vulkan {

	class GraphicsPipelineManager {
	public:
		VkResult create(
			const VkDevice& logicalDevice,
			const SwapchainManager& swapchain,
			const RenderPassManager& renderpass,
			const ShaderManager& shader,
			const DescriptorSetLayout& descriptorSetLayout
		);

		void destroy(const VkDevice& logicalDevice);

		const VkPipeline getGraphicsPipeline() const;

	private:
		VkPipelineLayout mVkPipelineLayout = nullptr;
		VkPipeline mVkGraphicsPipeline = nullptr;
	};

}