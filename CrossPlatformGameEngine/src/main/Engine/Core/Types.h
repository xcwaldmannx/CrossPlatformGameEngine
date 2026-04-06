#pragma once

#include <memory>

namespace ascen
{

	class CommandPool;
	using CommandPoolPtr = std::shared_ptr<CommandPool>;

	class DescriptorPool;
	using DescriptorPoolPtr = std::shared_ptr<DescriptorPool>;

	class DescriptorSetLayout;
	using DescriptorSetLayoutPtr = std::shared_ptr<DescriptorSetLayout>;

	class DescriptorSet;
	using DescriptorSetPtr = std::shared_ptr<DescriptorSet>;

	class Swapchain;
	using SwapchainPtr = std::shared_ptr<Swapchain>;

	class RenderPass;
	using RenderPassPtr = std::shared_ptr<RenderPass>;

	class GraphicsPipeline_I;
	using GraphicsPipelinePtr = std::shared_ptr<GraphicsPipeline_I>;

	class ComputePipeline_I;
	using ComputePipelinePtr = std::shared_ptr<ComputePipeline_I>;

	class PushConstant;
	using PushConstantPtr = std::shared_ptr<PushConstant>;

	class Buffer;
	using BufferPtr = std::shared_ptr<Buffer>;

	class Texture;
	using TexturePtr = std::shared_ptr<Texture>;

	class Sampler;
	using SamplerPtr = std::shared_ptr<Sampler>;

	class Vertex;
	using VertexPtr = std::shared_ptr<Vertex>;

	class FramePass;
	using FramePassPtr = std::shared_ptr<FramePass>;

}