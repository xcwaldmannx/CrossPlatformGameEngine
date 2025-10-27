#pragma once

#include "Pipeline_I.h"
#include "../../Utility/FileIO/FileIO.h"
#include "../Vertex/Vertex_I.h"

#include <concepts>
#include <stdexcept>

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	template<std::derived_from<Vertex_I> T>
	class Pipeline : public Pipeline_I
	{
	protected:
		Pipeline(
			const std::string vertexShaderFilepath,
			const std::string pixelShaderFilepath,
			const VkExtent2D& swapchainExtent,
			VkDescriptorSetLayout descriptorSetLayout,
			VkRenderPass renderPass)
			: Pipeline_I(
                vertexShaderFilepath,
			    pixelShaderFilepath)
		{
            mDescriptorSetLayouts = { descriptorSetLayout };

			handleVertexInputState();
			handleInputAssemblyState();
			handleDynamicState();
			handleViewportState(swapchainExtent);
			handleRasterizationState();
			handleMultisampleState();
			handleColorBlendState();
			handleDepthStencilState();
			handlePipelineLayoutAndInfo(renderPass);
		}

	public:
		void create(VkDevice device) override
		{
			// create vertex shader
			auto vertexShaderCode = FileIO::readFile(mVertexShaderFilepath);

			VkShaderModuleCreateInfo vertexShaderModuleInfo{};
			vertexShaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			vertexShaderModuleInfo.codeSize = vertexShaderCode.size();
			vertexShaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(vertexShaderCode.data());

			VkShaderModule vertexShaderModule;
			if (vkCreateShaderModule(device, &vertexShaderModuleInfo, nullptr, &vertexShaderModule) != VK_SUCCESS)
            {
				throw std::runtime_error("failed to create vertex shader stage!");
			}

			VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
			vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertexShaderStageInfo.module = vertexShaderModule;
			vertexShaderStageInfo.pName = "main";

			mShaderStages.push_back(vertexShaderStageInfo);

			// create pixel shader
			auto pixelShaderCode = FileIO::readFile(mPixelShaderFilepath);

			VkShaderModuleCreateInfo pixelShaderModuleInfo{};
			pixelShaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			pixelShaderModuleInfo.codeSize = pixelShaderCode.size();
			pixelShaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(pixelShaderCode.data());

			VkShaderModule pixelShaderModule;
			if (vkCreateShaderModule(device, &pixelShaderModuleInfo, nullptr, &pixelShaderModule) != VK_SUCCESS)
            {
				throw std::runtime_error("failed to create pixel shader stage!");
			}

			VkPipelineShaderStageCreateInfo pixelShaderStageInfo{};
			pixelShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			pixelShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			pixelShaderStageInfo.module = pixelShaderModule;
			pixelShaderStageInfo.pName = "main";

			mShaderStages.push_back(pixelShaderStageInfo);

			mCreateInfo.stageCount = static_cast<uint32_t>(mShaderStages.size());
			mCreateInfo.pStages = mShaderStages.data();

			// create pipeline layout and pipeline
			if (vkCreatePipelineLayout(
				device, &mLayoutInfo, nullptr, &mLayout) != VK_SUCCESS)
            {
				throw std::runtime_error("failed to create pipeline layout!");
			}

			mCreateInfo.layout = mLayout;

			if (vkCreateGraphicsPipelines(
				device, nullptr, 1, &mCreateInfo, nullptr, &mHandle) != VK_SUCCESS)
            {
				throw std::runtime_error("failed to create graphics pipeline!");
			}

			// destroy the shaders
			for (VkPipelineShaderStageCreateInfo& stage : mShaderStages)
			{
				vkDestroyShaderModule(device, stage.module, nullptr);
			}
		}

		void destroy(VkDevice device) override
		{
			vkDestroyPipeline(device, mHandle, nullptr);
			vkDestroyPipelineLayout(device, mLayout, nullptr);
		}

	private:
        void handleVertexInputState()
        {
            // Collect the per-vertex and per-instance binding descriptions
            auto vertexBindingDescription = T::getBindingDescription();
            auto vertexAttributeDescriptions = T::getAttributeDescriptions();

            //auto instanceBindingDescription = Instance::getBindingDescription();
            //auto instanceAttributeDescriptions = Instance::getAttributeDescriptions();

            mVertexInputBindingDescs.push_back(vertexBindingDescription);
            //mPipelineInfo.mVertexInputBindingInfo.push_back(instanceBindingDescription);

            mVertexInputAtrribDescs.insert(mVertexInputAtrribDescs.end(),
                vertexAttributeDescriptions.begin(), vertexAttributeDescriptions.end());
            //attributeDescriptions.insert(attributeDescriptions.end(),
            //    instanceAttributeDescriptions.begin(), instanceAttributeDescriptions.end());

            mVertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

            mVertexInputStateInfo.vertexBindingDescriptionCount =
                static_cast<uint32_t>(mVertexInputBindingDescs.size());

            mVertexInputStateInfo.pVertexBindingDescriptions =
                mVertexInputBindingDescs.data();

            mVertexInputStateInfo.vertexAttributeDescriptionCount =
                static_cast<uint32_t>(mVertexInputAtrribDescs.size());

            mVertexInputStateInfo.pVertexAttributeDescriptions =
                mVertexInputAtrribDescs.data();
        }

        void handleInputAssemblyState()
        {
            mInputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            mInputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            mInputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
        }

        void handleDynamicState()
        {
            mDynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            mDynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

            mDynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            mDynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(mDynamicStates.size());
            mDynamicStateInfo.pDynamicStates = mDynamicStates.data();
        }

        void handleViewportState(const VkExtent2D& swapchainExtent)
        {
            mViewport.x = 0.0f;
            mViewport.y = 0.0f;
            mViewport.width = (float)swapchainExtent.width;
            mViewport.height = (float)swapchainExtent.height;
            mViewport.minDepth = 0.0f;
            mViewport.maxDepth = 1.0f;

            mViewportScissor.offset = { 0, 0 };
            mViewportScissor.extent = swapchainExtent;

            mViewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            mViewportStateInfo.viewportCount = 1;
            mViewportStateInfo.pViewports = &mViewport;
            mViewportStateInfo.scissorCount = 1;
            mViewportStateInfo.pScissors = &mViewportScissor;
        }

        void handleRasterizationState()
        {
            mRasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            mRasterizationStateInfo.depthClampEnable = VK_FALSE;
            mRasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
            mRasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
            mRasterizationStateInfo.lineWidth = 1.0f;
            mRasterizationStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
            mRasterizationStateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            mRasterizationStateInfo.depthBiasEnable = VK_FALSE;
            mRasterizationStateInfo.depthBiasConstantFactor = 0.0f; // Optional
            mRasterizationStateInfo.depthBiasClamp = 0.0f; // Optional
            mRasterizationStateInfo.depthBiasSlopeFactor = 0.0f; // Optional
        }

        void handleMultisampleState()
        {
            mMultisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            mMultisampleStateInfo.sampleShadingEnable = VK_FALSE;
            mMultisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            mMultisampleStateInfo.minSampleShading = 1.0f; // Optional
            mMultisampleStateInfo.pSampleMask = nullptr; // Optional
            mMultisampleStateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
            mMultisampleStateInfo.alphaToOneEnable = VK_FALSE; // Optional
        }

        void handleColorBlendState()
        {
            mColorBlendAttachmentState.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;
            mColorBlendAttachmentState.blendEnable = VK_TRUE;
            mColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            mColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            mColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
            mColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            mColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            mColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

            mColorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            mColorBlendStateInfo.logicOpEnable = VK_FALSE;
            mColorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
            mColorBlendStateInfo.attachmentCount = 1;
            mColorBlendStateInfo.pAttachments = &mColorBlendAttachmentState;
            mColorBlendStateInfo.blendConstants[0] = 0.0f; // Optional
            mColorBlendStateInfo.blendConstants[1] = 0.0f; // Optional
            mColorBlendStateInfo.blendConstants[2] = 0.0f; // Optional
            mColorBlendStateInfo.blendConstants[3] = 0.0f; // Optional
        }

        void handleDepthStencilState()
        {
            mDepthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            mDepthStencilStateInfo.depthTestEnable = VK_TRUE;
            mDepthStencilStateInfo.depthWriteEnable = VK_TRUE;
            mDepthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
            mDepthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
            mDepthStencilStateInfo.minDepthBounds = 0.0f; // Optional
            mDepthStencilStateInfo.maxDepthBounds = 1.0f; // Optional
            mDepthStencilStateInfo.stencilTestEnable = VK_FALSE;
            mDepthStencilStateInfo.front = {}; // Optional
            mDepthStencilStateInfo.back = {}; // Optional
        }

        void handlePipelineLayoutAndInfo(VkRenderPass renderPass)
        {
            // pipeline layout creation
            mLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            mLayoutInfo.setLayoutCount = static_cast<uint32_t>(mDescriptorSetLayouts.size());
            mLayoutInfo.pSetLayouts = mDescriptorSetLayouts.data();
            mLayoutInfo.pushConstantRangeCount = 0; // Optional
            mLayoutInfo.pPushConstantRanges = nullptr; // Optional

            // pipeline creation
            mCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            mCreateInfo.stageCount = static_cast<uint32_t>(mShaderStages.size());
            mCreateInfo.pStages = mShaderStages.data();
            mCreateInfo.pVertexInputState = &mVertexInputStateInfo;
            mCreateInfo.pInputAssemblyState = &mInputAssemblyInfo;
            mCreateInfo.pDynamicState = &mDynamicStateInfo;
            mCreateInfo.pViewportState = &mViewportStateInfo;
            mCreateInfo.pRasterizationState = &mRasterizationStateInfo;
            mCreateInfo.pMultisampleState = &mMultisampleStateInfo;
            mCreateInfo.pColorBlendState = &mColorBlendStateInfo;
            mCreateInfo.pDepthStencilState = &mDepthStencilStateInfo;
            mCreateInfo.renderPass = renderPass;
            mCreateInfo.subpass = 0;
            mCreateInfo.basePipelineHandle = nullptr; // Optional
            mCreateInfo.basePipelineIndex = -1; // Optional
        }

	protected:
		std::vector<VkPipelineShaderStageCreateInfo> mShaderStages;
		std::vector<VkVertexInputBindingDescription>  mVertexInputBindingDescs;
		std::vector<VkVertexInputAttributeDescription> mVertexInputAtrribDescs;
        std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
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

	};
}