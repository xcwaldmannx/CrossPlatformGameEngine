#pragma once

#include "../Registry_I.h"
#include "../../Core/Types.h"

#include "../../Core/VulkanContext.h"
#include "../../Core/RenderContext.h"
#include "../Vertex/VertexRegistry.h"
#include "../Descriptor/DescriptorRegistry.h"

#include "../../Pipeline/GraphicsPipeline/GraphicsPipeline.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace ascen
{

	struct PipelineEntry
	{
		std::string mName;
		std::vector<std::string> mDescriptorSetLayouts;
	};

	struct GraphicsPipelineEntry : PipelineEntry
	{
		std::string mVertexShader;
		std::string mPixelShader;
		std::string mVertex;

		GraphicsPipelineParams mParams;
	};

	struct ComputePipelineEntry : PipelineEntry
	{
		std::string mComputeShader;

		ComputePipelineParams mParams;
	};

	class PipelineRegistry : public Registry_I
	{
	public:
		PipelineRegistry(
			const VulkanContext& vulkanContext,
			const RenderContext& renderContext,
			const VertexRegistry& vertexRegistry,
			const DescriptorRegistry& descriptorRegistry);

		void registerGraphicsPipeline(GraphicsPipelineEntry entry);
		void registerComputePipeline(ComputePipelineEntry entry);

		void pushConstants(
			const std::string& pipelineName,
			const std::string& pushConstantName,
			const void* data);

		void uploadAllPushConstants(const VkCommandBuffer commandBuffer);

		void reconstruct() override;

		void cleanup();

	private:
		bool isRegistered(const std::string& name) const;
		bool pushConstantExists(const std::string& name) const;
		bool graphicsPipelineExists(const std::string& name) const;
		bool computePipelineExists(const std::string& name) const;

	private:
		const VkDevice mDevice;
		const VkQueue mGraphicsQueue;
		const CommandPoolPtr& mCommandPool;

		const PushConstantFactory& mPushConstantFactory;
		const GraphicsPipelineFactory& mGraphicsPipelineFactory;
		const ComputePipelineFactory& mComputePipelineFactory;
		const SwapchainPtr& mSwapchain;
		const RenderPassPtr& mRenderPass;
		const VertexRegistry& mVertexRegistry;
		const DescriptorRegistry& mDescriptorRegistry;

		std::vector<std::string> mRegisteredNames;

		std::vector<GraphicsPipelineEntry> mGraphicsEntries;
		std::vector<ComputePipelineEntry> mComputeEntries;

		std::unordered_map<std::string, PushConstantPtr> mPushConstants;
		std::unordered_map<std::string, GraphicsPipelinePtr> mGraphicsPipelines;
		std::unordered_map<std::string, ComputePipelinePtr> mComputePipelines;

		std::vector<std::function<void(const VkCommandBuffer)>> mPushConstantUploads;

		friend class PipelineRegistryBackend;
	};

}
