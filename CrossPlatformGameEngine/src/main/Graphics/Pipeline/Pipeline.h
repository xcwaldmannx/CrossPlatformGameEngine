#pragma once

#include "../HandleManager/Handle.h"

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class Pipeline : public Handle<VkPipeline>
	{
	protected:
		Pipeline(
			const std::string vertexShaderFilepath,
			const std::string pixelShaderFilepath,
			const VkExtent2D& swapchainExtent,
			VkDescriptorSetLayout& descriptorSetLayout,
			VkRenderPass renderPass);

	public:
		void create(VkDevice device) override;
		void destroy(VkDevice device) override;

		VkPipelineLayout getLayout();

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

	protected:
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
		VkGraphicsPipelineCreateInfo mCreateInfo{};

	private:
		VkPipelineLayout mLayout;
		const std::string mVertexShaderFilepath;
		const std::string mPixelShaderFilepath;

	};
}