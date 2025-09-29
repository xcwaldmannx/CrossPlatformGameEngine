#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	struct PipelineInfo
	{
		std::vector<VkPipelineShaderStageCreateInfo> mShaderStages;
		std::vector<VkVertexInputBindingDescription>  mVertexInputBindingDescs;
		std::vector<VkVertexInputAttributeDescription> mVertexInputAtrribDescs;
		VkPipelineVertexInputStateCreateInfo mVertexInputStateInfo{};
		VkPipelineInputAssemblyStateCreateInfo mInputAssemblyInfo{};
		std::vector<VkDynamicState> mDynamicStates;
		VkPipelineDynamicStateCreateInfo mDynamicStateInfo{};
		VkViewport mViewport{};
		VkRect2D mViewportScissor{};
		VkPipelineViewportStateCreateInfo mViewportStateInfo{};
		VkPipelineRasterizationStateCreateInfo mRasterizationStateInfo{};
		VkPipelineMultisampleStateCreateInfo mMultisampleStateInfo{};
		VkPipelineColorBlendAttachmentState mColorBlendAttachmentState{};
		VkPipelineColorBlendStateCreateInfo mColorBlendStateInfo{};
		VkPipelineDepthStencilStateCreateInfo mDepthStencilStateInfo{};
		VkPipelineLayoutCreateInfo mLayoutInfo{};
		VkGraphicsPipelineCreateInfo mPipelineInfo{};
	};

	class Pipeline
	{
	protected:
		Pipeline(
			const std::string vertexShaderFilepath,
			const std::string pixelShaderFilepath,
			const VkExtent2D& swapchainExtent,
			VkDescriptorSetLayout& descriptorSetLayout,
			VkRenderPass renderPass);

	public:
		void create(VkDevice device);
		void destroy(VkDevice device);

	private:
		void handleVertexInputState();
		void handleInputAssemblyState();
		void handleDynamicState();
		void handleViewportState(const VkExtent2D& swapchainExtent);
		void handleRasterizationState();
		void handleMultisampleState();
		void handleColorBlendState();
		void handleDepthStencilState();
		void handlePipelineLayoutAndInfo(VkDescriptorSetLayout& descriptorSetLayout, VkRenderPass renderPass);

	public:
		VkPipelineLayout mLayout;
		VkPipeline mPipeline;

	protected:
		PipelineInfo mPipelineInfo{};

	private:
		const std::string mVertexShaderFilepath;
		const std::string mPixelShaderFilepath;

	};
}