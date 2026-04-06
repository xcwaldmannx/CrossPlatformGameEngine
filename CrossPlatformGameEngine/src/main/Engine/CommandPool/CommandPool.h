#pragma once

#include "../Core/Types.h"
#include "../Handle/Handle.h"

#include "../FrameGraph/FramePass/GpuFramePass/GraphicsGpuFramePass/GraphicsGpuFramePass.h"
#include "../FrameGraph/FramePass/GpuFramePass/ComputeGpuFramePass/ComputeGpuFramePass.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class CommandPoolFactory;
	class RenderPass;
	class Swapchain;
	class FrameGraph;

	struct CommandDrawData
	{
		uint32_t mMeshCount = 0;
		std::vector<uint32_t> mVertexOffsets;
		std::vector<uint32_t> mIndexCounts;
		std::vector<uint32_t> mIndexOffsets;
		std::vector<uint32_t> mTransformOffsets;
	};
	
	class CommandPool : public Handle<VkCommandPool>
	{
	private:
		CommandPool(
			VkDevice device,
			uint32_t queueFamilyIndex);

	public:
		void destroy(VkDevice device) override;

		VkCommandBuffer beginCommand(uint32_t frameIndex);

		void endCommand(VkCommandBuffer buffer);

		void beginRenderPass(
			VkCommandBuffer commandBuffer,
			uint32_t currentImage,
			const RenderPassPtr& renderPass,
			const SwapchainPtr& swapchain);

		void endRenderPass(VkCommandBuffer commandBuffer);

		void recordGraphics(
			VkCommandBuffer commandBuffer,
			const GraphicsGpuFramePass* framePass,
			uint32_t frameIndex,
			VkBuffer indirectBuffer,
			uint32_t drawCommandCount);

		void recordCompute(
			VkCommandBuffer commandBuffer,
			const ComputeGpuFramePass* framePass,
			uint32_t currentFrame);

		VkCommandBuffer beginSingle(VkDevice device);
		void endSingle(VkDevice device, VkQueue queue, VkCommandBuffer buffer);

		const VkCommandBuffer* getBufferIndex(size_t index) const;

	private:
		std::vector<VkCommandBuffer> mCommandBuffers;

		friend class CommandPoolFactory;
	};

}
