#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{


	class RenderGraph
	{
	public:
		struct Pass
		{
			std::vector<VkBuffer> mVertexBuffers;
			VkBuffer mIndexBuffer = VK_NULL_HANDLE;
			std::vector<VkDescriptorSet> mDescriptorSets;
			VkPipeline mPipeline = VK_NULL_HANDLE;
			VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
		};

		RenderGraph();

		void addPass(const Pass& pass);

		void compile();

		const std::vector<Pass>& getExecutions() const;

	private:
		std::vector<Pass> mPasses;
		std::vector<Pass> mExecutions;
	};

}
