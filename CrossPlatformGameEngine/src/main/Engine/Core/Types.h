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

	using IndexedIndirectDraw = VkDrawIndexedIndirectCommand;
	using IndirectDraw = VkDrawIndirectCommand;

	template<typename T, typename U>
	concept Derived = std::is_base_of_v<U, T>;

	namespace renderpass
	{
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

	namespace pipeline
	{
		struct PushConstantRange // might need rework
		{
			uint32_t mOffset = 0;
			uint32_t mSize = 0;
		};

		struct Params
		{
			PushConstantRange mPushConstantRange{};
		};

		struct GraphicsParams : Params
		{
			uint32_t mTopologyMode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			uint32_t mPolygonMode = VK_POLYGON_MODE_FILL;
			uint32_t mCullMode = VK_CULL_MODE_BACK_BIT;
		};

		struct ComputeParams : Params {};
	}

	namespace registry
	{
		using Resource = std::shared_ptr<void>;

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
			uint64_t mPoolId = 0;
			uint64_t mLayoutId = 0;
			std::vector<DescriptorResource> mResources;
		};

		struct BufferEntry : Entry
		{
			uint32_t mCapacity = 1; // default must be > 0
			uint32_t mStride = 1;   // default must be > 0
			BufferUsageFlags mUsageFlags = 0;
			BufferMemoryFlags mMemoryFlags = 0;
		};

		struct SamplerEntry : Entry {};

		struct TextureEntry : Entry
		{
			TextureType mType = TextureType::NONE;
			uint32_t mWidth = 0;
			uint32_t mHeight = 0;
			uint32_t mLayers = 0;
		};

		struct RenderPassEntry : Entry
		{
			std::vector<renderpass::Attachment> mAttachments;
			std::vector<renderpass::SubPass> mSubPasses;
			std::vector<renderpass::SubPassDependency> mSubPassDependencies;
		};

		struct RenderTargetEntry : Entry
		{
			Format mFormat;
			std::vector<VkImage> mImages;
			uint32_t mImageCount;
		};

		struct PipelineEntry : Entry
		{
			std::vector<uint64_t> mDescriptorSetLayoutIds;
		};

		struct GraphicsPipelineEntry : PipelineEntry
		{
			std::string mVertexShaderPath;
			std::string mPixelShaderPath;
			uint64_t mVertexId;
			pipeline::GraphicsParams mParams;
		};

		struct ComputePipelineEntry : PipelineEntry
		{
			std::string mComputeShaderPath;
			pipeline::ComputeParams mParams;
		};
	}

}