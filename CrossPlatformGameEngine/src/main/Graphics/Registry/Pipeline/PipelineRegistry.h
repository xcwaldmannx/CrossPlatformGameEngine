#pragma once

#include "../Registry_I.h"
#include "../../Types.h"

#include "../../VulkanContext.h"
#include "../../RenderContext.h"
#include "../Vertex/VertexRegistry.h"
#include "../Descriptor/DescriptorRegistry.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace ascen
{

	struct GraphicsPipelineEntry
	{
		std::string mName;
		std::string mVertexShader;
		std::string mPixelShader;
		std::string mVertex;
		std::vector<std::string> mDescriptorSetLayouts;
	};

	struct ComputePipelineEntry
	{
		std::string mName;
		std::string mComputeShader;
	};

	class PipelineRegistry : public Registry_I
	{
	public:
		PipelineRegistry(
			VulkanContext& vulkanContext,
			RenderContext& renderContext,
			VertexRegistry& vertexRegistry,
			DescriptorRegistry& descriptorRegistry);

		void registerGraphicsPipeline(GraphicsPipelineEntry entry);
		void registerComputePipeline(ComputePipelineEntry entry);

		void reconstruct() override;

		void cleanup();

	private:
		bool isRegistered(const std::string& name) const;

	private:
		const VkDevice mDevice;
		const GraphicsPipelineFactory& mGraphicsPipelineFactory;
		const ComputePipelineFactory& mComputePipelineFactory;
		const SwapchainPtr& mSwapchain;
		const RenderPassPtr& mRenderPass;
		VertexRegistry& mVertexRegistry;
		DescriptorRegistry& mDescriptorRegistry;

		std::vector<std::string> mRegisteredNames;

		std::vector<GraphicsPipelineEntry> mGraphicsEntries;
		std::vector<ComputePipelineEntry> mComputeEntries;

		std::unordered_map<std::string, GraphicsPipelinePtr> mGraphicsPipelines;
		std::unordered_map<std::string, ComputePipelinePtr> mComputePipelines;
	};

}
