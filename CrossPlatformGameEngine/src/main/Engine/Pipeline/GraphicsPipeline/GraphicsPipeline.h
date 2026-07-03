#pragma once

#include "../../Core/Types.h"

#include "../../../Utility/FileIO/FileIO.h"

#include "GraphicsPipeline_I.h"
#include "../../Vertex/Vertex.h"
#include "../../Descriptor/Layout/DescriptorSetLayout.h"
#include "../../Swapchain/Swapchain.h"
#include "../../RenderTarget/RenderPass/RenderPass.h"

#include <concepts>
#include <stdexcept>

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    class GraphicsPipeline : public GraphicsPipeline_I
    {
    protected:
        GraphicsPipeline(
            const VkDevice device,
            const pipeline::GraphicsParams& params,
            const std::string& vertexShaderFilepath,
            const std::string& pixelShaderFilepath,
            const VertexPtr& vertex,
            const std::vector<DescriptorSetLayoutPtr>& descriptorSetLayouts,
            const SwapchainPtr& swapchain,
            const RenderPassPtr& renderPass) :
            GraphicsPipeline_I(
                params,
                vertexShaderFilepath,
                pixelShaderFilepath)
        {
            handleVertexInput(vertex);
            handleInputAssembly();
            handleDynamic();
            handleViewport(swapchain->getExtent());
            handleRasterization();
            handleMultisample();
            handleColorBlend();
            handleDepthStencil();

            std::vector<VkDescriptorSetLayout> vkLayouts;
            for (const auto& layout : descriptorSetLayouts)
            {
                vkLayouts.push_back(layout->handle());
            }

            handlePipeline(vkLayouts, renderPass->handle());

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
                throw std::runtime_error("failed to create graphics pipeline layout!");
            }

            mCreateInfo.layout = mLayout;

            VkResult res = vkCreateGraphicsPipelines(device, nullptr, 1, &mCreateInfo, nullptr, &mHandle);
            if (res != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create graphics pipeline!");
            }

            // destroy the shaders
            for (VkPipelineShaderStageCreateInfo& stage : mShaderStages)
            {
                vkDestroyShaderModule(device, stage.module, nullptr);
            }
        }

    public:
        void destroy(VkDevice device) override
        {
            vkDestroyPipeline(device, mHandle, nullptr);
            vkDestroyPipelineLayout(device, mLayout, nullptr);
        }

    private:
        void handleVertexInput(const VertexPtr& vertex)
        {
            auto& vertexBindingDescription = vertex->getBinding();
            auto& vertexAttributeDescriptions = vertex->getAttributes();

            mVertexInputBindingDescs.push_back(vertexBindingDescription);

            mVertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

            mVertexInputStateInfo.vertexBindingDescriptionCount =
                static_cast<uint32_t>(mVertexInputBindingDescs.size());
            mVertexInputStateInfo.pVertexBindingDescriptions = &vertexBindingDescription;

            mVertexInputStateInfo.vertexAttributeDescriptionCount =
                static_cast<uint32_t>(vertexAttributeDescriptions.size());

            mVertexInputStateInfo.pVertexAttributeDescriptions =
                vertexAttributeDescriptions.data();
        }

        void handleInputAssembly()
        {
            mInputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            mInputAssemblyInfo.topology = static_cast<VkPrimitiveTopology>(mParams.mTopologyMode);
            mInputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
        }

        void handleDynamic()
        {
            mDynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            mDynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

            mDynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            mDynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(mDynamicStates.size());
            mDynamicStateInfo.pDynamicStates = mDynamicStates.data();
        }

        void handleViewport(const VkExtent2D& swapchainExtent)
        {
            mViewport.x = 0.0f;
            mViewport.y = 0.0f;
            mViewport.width = static_cast<float>(swapchainExtent.width);
            mViewport.height = static_cast<float>(swapchainExtent.height);
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

        void handleRasterization()
        {
            mRasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            mRasterizationStateInfo.depthClampEnable = VK_FALSE;
            mRasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
            mRasterizationStateInfo.polygonMode = static_cast<VkPolygonMode>(mParams.mPolygonMode);
            mRasterizationStateInfo.lineWidth = 1.0f;
            mRasterizationStateInfo.cullMode = static_cast<VkCullModeFlags>(mParams.mCullMode);
            mRasterizationStateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            mRasterizationStateInfo.depthBiasEnable = VK_FALSE;
            mRasterizationStateInfo.depthBiasConstantFactor = 0.0f; // Optional
            mRasterizationStateInfo.depthBiasClamp = 0.0f; // Optional
            mRasterizationStateInfo.depthBiasSlopeFactor = 0.0f; // Optional
        }

        void handleMultisample()
        {
            mMultisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            mMultisampleStateInfo.sampleShadingEnable = VK_FALSE;
            mMultisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            mMultisampleStateInfo.minSampleShading = 1.0f; // Optional
            mMultisampleStateInfo.pSampleMask = nullptr; // Optional
            mMultisampleStateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
            mMultisampleStateInfo.alphaToOneEnable = VK_FALSE; // Optional
        }

        void handleColorBlend()
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

        void handleDepthStencil()
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

        void handlePipeline(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, VkRenderPass renderPass)
        {
            mLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            mLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
            mLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
            mLayoutInfo.pushConstantRangeCount = 0;
            mLayoutInfo.pPushConstantRanges = nullptr;

            const auto& pc = mParams.mPushConstantRange;

            if (pc.mSize > 0)
            {
                mPushConstantRange = VkPushConstantRange(
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    mParams.mPushConstantRange.mOffset,
                    mParams.mPushConstantRange.mSize);

                mLayoutInfo.pushConstantRangeCount = 1;
                mLayoutInfo.pPushConstantRanges = &mPushConstantRange;
            }


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
        }

    protected:
        std::vector<VkPipelineShaderStageCreateInfo> mShaderStages;
        std::vector<VkVertexInputBindingDescription>  mVertexInputBindingDescs;
        std::vector<VkVertexInputAttributeDescription> mVertexInputAtrribDescs;
        std::vector<VkDynamicState> mDynamicStates;

        VkPipelineVertexInputStateCreateInfo mVertexInputStateInfo{};
        VkPipelineInputAssemblyStateCreateInfo mInputAssemblyInfo{};
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

        friend class GraphicsPipelineFactory;
    };
}