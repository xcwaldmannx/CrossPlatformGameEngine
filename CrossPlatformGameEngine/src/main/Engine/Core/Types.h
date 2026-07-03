#pragma once

#include "Values.h"

#include <memory>
#include <unordered_map>
#include <vector>

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

	class FrameBuffer;
	using FrameBufferPtr = std::shared_ptr<FrameBuffer>;

	class RenderPass;
	using RenderPassPtr = std::shared_ptr<RenderPass>;

	class RenderTarget;
	using RenderTargetPtr = std::shared_ptr<RenderTarget>;

	class FramePass;
	using FramePassPtr = std::shared_ptr<FramePass>;

	using VertexBinding = VkVertexInputBindingDescription;
	using VertexAttribute = VkVertexInputAttributeDescription;

	namespace registry
	{
		struct Entry
		{
			std::string mName;
		};

		struct VertexEntry : Entry
		{
			VertexBinding mBinding;
			std::vector<VertexAttribute> mAttributes;
		};

		struct DescriptorPoolEntry : Entry
		{
			std::unordered_map<DescriptorType, uint32_t> mDescriptorTypeCounts;
		};

		struct DescriptorLocation
		{
			uint32_t mSlot = 0;
			DescriptorType mType = SSBO;
		};

		struct DescriptorBinding
		{
			DescriptorLocation mLocation;
			uint32_t mStage = 0;
		};

		struct DescriptorResource
		{
			uint64_t mResourceId = 0;
			DescriptorLocation mLocation;
			uint64_t mSize = 0;
		};

		struct DescriptorSetLayoutEntry : Entry
		{
			std::vector<DescriptorBinding> mBindings;
		};

		struct DescriptorSetEntry : Entry
		{
			std::vector<DescriptorResource> mResources;
		};

		using Resource = std::shared_ptr<void>;
	}

	struct Attachment
	{
		AttachmentType mType;
		Format mFormat;
		LoadOp mLoadOp;
		StoreOp mStoreOp;
		LoadOp mDepthStencilLoadOp;
		StoreOp mDepthStencilStoreOp;
	};

	struct SubPassDependency
	{
		uint32_t mSrcSubpass;
		uint32_t mDstSubpass;
		PipelineStageFlag mSrcStageMask;
		PipelineStageFlag mDstStageMask;
		AccessMaskFlag mSrcAccessMask;
		AccessMaskFlag mDstAccessMask;
	};

	struct SubPass
	{
		BindPoint mBindPoint;
		std::vector<uint32_t> mColorAttachmentIndices;
		std::vector<uint32_t> mInputAttachmentIndices;
		int32_t mDepthAttachmentIndex;
	};

}