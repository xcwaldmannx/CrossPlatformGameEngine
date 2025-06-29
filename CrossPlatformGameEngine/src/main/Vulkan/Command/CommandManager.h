#pragma once

#include "../QueueFamily/QueueFamilyManager.h"

#include "../Swapchain/SwapchainManager.h"
#include "../RenderPass/RenderPassManager.h"
#include "../GraphicsPipeline/GraphicsPipelineManager.h"

#include "../ResourceManager/Mesh/Mesh.h"

#include <vulkan/vulkan.h>

namespace vulkan {

	class CommandManager {
	public:
		VkResult create(const VkDevice& logicalDevice, const QueueFamilyManager& queueFamilyManager);
		void destroy(const VkDevice& logicalDevice);

		void addMesh(const Mesh& mesh);

		void record(
			uint32_t frameIndex,
			uint32_t imageIndex,
			const RenderPassManager& renderpass,
			const SwapchainManager& swapchain,
			const GraphicsPipelineManager& graphicsPipeline
		);

		const VkCommandPool& getCommandPool() const;
		const VkCommandBuffer& getCommandBuffer(size_t index) const;

	private:
		VkCommandPool mVkCommandPool = nullptr;
		std::vector<VkCommandBuffer> mCommandBuffers;

		std::vector<Mesh> mMeshes;

	};

}
