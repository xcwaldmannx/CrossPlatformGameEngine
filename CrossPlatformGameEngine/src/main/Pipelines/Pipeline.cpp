#include "Pipeline.h"

#include <stdexcept>

#include "../Utility/FileIO/FileIO.h"
#include "../GraphicsPipeline/Vertex/Vertex.h"

using namespace ascen;

Pipeline::Pipeline(
	const std::string vertexShaderFilepath,
	const std::string pixelShaderFilepath,
    const VkExtent2D& swapchainExtent,
    VkDescriptorSetLayout& descriptorSetLayout,
    VkRenderPass renderPass)
    : mVertexShaderFilepath(vertexShaderFilepath),
      mPixelShaderFilepath(pixelShaderFilepath)
{
    handleVertexInputState();
    handleInputAssemblyState();
    handleDynamicState();
    handleViewportState(swapchainExtent);
    handleRasterizationState();
    handleMultisampleState();
    handleColorBlendState();
    handleDepthStencilState();
    handlePipelineLayoutAndInfo(descriptorSetLayout, renderPass);
}

void Pipeline::create(VkDevice device)
{
    // create vertex shader
    auto vertexShaderCode = FileIO::readFile(mVertexShaderFilepath);

    VkShaderModuleCreateInfo vertexShaderModuleInfo{};
    vertexShaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vertexShaderModuleInfo.codeSize = vertexShaderCode.size();
    vertexShaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(vertexShaderCode.data());

    VkShaderModule vertexShaderModule;
    if (vkCreateShaderModule(device, &vertexShaderModuleInfo, nullptr, &vertexShaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex shader stage!");
    }

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};

    vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageInfo.module = vertexShaderModule;
    vertexShaderStageInfo.pName = "main";

    mPipelineInfo.mShaderStages.push_back(vertexShaderStageInfo);

    // create pixel shader
    auto pixelShaderCode = FileIO::readFile(mPixelShaderFilepath);

    VkShaderModuleCreateInfo pixelShaderModuleInfo{};
    pixelShaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    pixelShaderModuleInfo.codeSize = pixelShaderCode.size();
    pixelShaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(pixelShaderCode.data());

    VkShaderModule pixelShaderModule;
    if (vkCreateShaderModule(device, &pixelShaderModuleInfo, nullptr, &pixelShaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pixel shader stage!");
    }

    VkPipelineShaderStageCreateInfo pixelShaderStageInfo{};

    pixelShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pixelShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pixelShaderStageInfo.module = pixelShaderModule;
    pixelShaderStageInfo.pName = "main";

    mPipelineInfo.mShaderStages.push_back(pixelShaderStageInfo);

    mPipelineInfo.mPipelineInfo.stageCount = static_cast<uint32_t>(mPipelineInfo.mShaderStages.size());
    mPipelineInfo.mPipelineInfo.pStages = mPipelineInfo.mShaderStages.data();

    // create pipeline layout and pipeline
    if (vkCreatePipelineLayout(
        device, &mPipelineInfo.mLayoutInfo, nullptr, &mLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    mPipelineInfo.mPipelineInfo.layout = mLayout;

    if (vkCreateGraphicsPipelines(
        device, nullptr, 1, &mPipelineInfo.mPipelineInfo, nullptr, &mPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // destroy the shaders
    for (VkPipelineShaderStageCreateInfo& stage : mPipelineInfo.mShaderStages)
    {
        vkDestroyShaderModule(device, stage.module, nullptr);
    }
}

void Pipeline::destroy(VkDevice device)
{
    vkDestroyPipeline(device, mPipeline, nullptr);
    vkDestroyPipelineLayout(device, mLayout, nullptr);
}

void Pipeline::handleVertexInputState()
{
    // Collect the per-vertex and per-instance binding descriptions
    auto vertexBindingDescription = Vertex::getBindingDescription();
    auto vertexAttributeDescriptions = Vertex::getAttributeDescriptions();

    //auto instanceBindingDescription = Instance::getBindingDescription();
    //auto instanceAttributeDescriptions = Instance::getAttributeDescriptions();

    mPipelineInfo.mVertexInputBindingDescs.push_back(vertexBindingDescription);
    //mPipelineInfo.mVertexInputBindingInfo.push_back(instanceBindingDescription);

    mPipelineInfo.mVertexInputAtrribDescs.insert(mPipelineInfo.mVertexInputAtrribDescs.end(),
        vertexAttributeDescriptions.begin(), vertexAttributeDescriptions.end());
    //attributeDescriptions.insert(attributeDescriptions.end(),
    //    instanceAttributeDescriptions.begin(), instanceAttributeDescriptions.end());

    mPipelineInfo.mVertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    mPipelineInfo.mVertexInputStateInfo.vertexBindingDescriptionCount =
        static_cast<uint32_t>(mPipelineInfo.mVertexInputBindingDescs.size());

    mPipelineInfo.mVertexInputStateInfo.pVertexBindingDescriptions =
        mPipelineInfo.mVertexInputBindingDescs.data();

    mPipelineInfo.mVertexInputStateInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(mPipelineInfo.mVertexInputAtrribDescs.size());

    mPipelineInfo.mVertexInputStateInfo.pVertexAttributeDescriptions =
        mPipelineInfo.mVertexInputAtrribDescs.data();
}

void Pipeline::handleInputAssemblyState()
{
    mPipelineInfo.mInputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    mPipelineInfo.mInputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    mPipelineInfo.mInputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
}

void Pipeline::handleDynamicState()
{
    mPipelineInfo.mDynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    mPipelineInfo.mDynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

    mPipelineInfo.mDynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    mPipelineInfo.mDynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(mPipelineInfo.mDynamicStates.size());
    mPipelineInfo.mDynamicStateInfo.pDynamicStates = mPipelineInfo.mDynamicStates.data();
}

void Pipeline::handleViewportState(const VkExtent2D& swapchainExtent)
{
    mPipelineInfo.mViewport.x = 0.0f;
    mPipelineInfo.mViewport.y = 0.0f;
    mPipelineInfo.mViewport.width = (float)swapchainExtent.width;
    mPipelineInfo.mViewport.height = (float)swapchainExtent.height;
    mPipelineInfo.mViewport.minDepth = 0.0f;
    mPipelineInfo.mViewport.maxDepth = 1.0f;

    mPipelineInfo.mViewportScissor.offset = { 0, 0 };
    mPipelineInfo.mViewportScissor.extent = swapchainExtent;

    mPipelineInfo.mViewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    mPipelineInfo.mViewportStateInfo.viewportCount = 1;
    mPipelineInfo.mViewportStateInfo.pViewports = &mPipelineInfo.mViewport;
    mPipelineInfo.mViewportStateInfo.scissorCount = 1;
    mPipelineInfo.mViewportStateInfo.pScissors = &mPipelineInfo.mViewportScissor;
}

void Pipeline::handleRasterizationState()
{
    mPipelineInfo.mRasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    mPipelineInfo.mRasterizationStateInfo.depthClampEnable = VK_FALSE;
    mPipelineInfo.mRasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
    mPipelineInfo.mRasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    mPipelineInfo.mRasterizationStateInfo.lineWidth = 1.0f;
    mPipelineInfo.mRasterizationStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    mPipelineInfo.mRasterizationStateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    mPipelineInfo.mRasterizationStateInfo.depthBiasEnable = VK_FALSE;
    mPipelineInfo.mRasterizationStateInfo.depthBiasConstantFactor = 0.0f; // Optional
    mPipelineInfo.mRasterizationStateInfo.depthBiasClamp = 0.0f; // Optional
    mPipelineInfo.mRasterizationStateInfo.depthBiasSlopeFactor = 0.0f; // Optional
}

void Pipeline::handleMultisampleState()
{
    mPipelineInfo.mMultisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    mPipelineInfo.mMultisampleStateInfo.sampleShadingEnable = VK_FALSE;
    mPipelineInfo.mMultisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    mPipelineInfo.mMultisampleStateInfo.minSampleShading = 1.0f; // Optional
    mPipelineInfo.mMultisampleStateInfo.pSampleMask = nullptr; // Optional
    mPipelineInfo.mMultisampleStateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    mPipelineInfo.mMultisampleStateInfo.alphaToOneEnable = VK_FALSE; // Optional
}

void Pipeline::handleColorBlendState()
{
    mPipelineInfo.mColorBlendAttachmentState.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    mPipelineInfo.mColorBlendAttachmentState.blendEnable = VK_TRUE;
    mPipelineInfo.mColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    mPipelineInfo.mColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    mPipelineInfo.mColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    mPipelineInfo.mColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    mPipelineInfo.mColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    mPipelineInfo.mColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    mPipelineInfo.mColorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    mPipelineInfo.mColorBlendStateInfo.logicOpEnable = VK_FALSE;
    mPipelineInfo.mColorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
    mPipelineInfo.mColorBlendStateInfo.attachmentCount = 1;
    mPipelineInfo.mColorBlendStateInfo.pAttachments = &mPipelineInfo.mColorBlendAttachmentState;
    mPipelineInfo.mColorBlendStateInfo.blendConstants[0] = 0.0f; // Optional
    mPipelineInfo.mColorBlendStateInfo.blendConstants[1] = 0.0f; // Optional
    mPipelineInfo.mColorBlendStateInfo.blendConstants[2] = 0.0f; // Optional
    mPipelineInfo.mColorBlendStateInfo.blendConstants[3] = 0.0f; // Optional
}

void Pipeline::handleDepthStencilState()
{
    mPipelineInfo.mDepthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    mPipelineInfo.mDepthStencilStateInfo.depthTestEnable = VK_TRUE;
    mPipelineInfo.mDepthStencilStateInfo.depthWriteEnable = VK_TRUE;
    mPipelineInfo.mDepthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    mPipelineInfo.mDepthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
    mPipelineInfo.mDepthStencilStateInfo.minDepthBounds = 0.0f; // Optional
    mPipelineInfo.mDepthStencilStateInfo.maxDepthBounds = 1.0f; // Optional
    mPipelineInfo.mDepthStencilStateInfo.stencilTestEnable = VK_FALSE;
    mPipelineInfo.mDepthStencilStateInfo.front = {}; // Optional
    mPipelineInfo.mDepthStencilStateInfo.back = {}; // Optional
}

void Pipeline::handlePipelineLayoutAndInfo(VkDescriptorSetLayout& descriptorSetLayout, VkRenderPass renderPass)
{
    // pipeline layout creation
    mPipelineInfo.mLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    mPipelineInfo.mLayoutInfo.setLayoutCount = 1;
    mPipelineInfo.mLayoutInfo.pSetLayouts = &descriptorSetLayout;
    mPipelineInfo.mLayoutInfo.pushConstantRangeCount = 0; // Optional
    mPipelineInfo.mLayoutInfo.pPushConstantRanges = nullptr; // Optional

    // pipeline creation
    mPipelineInfo.mPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    mPipelineInfo.mPipelineInfo.stageCount = static_cast<uint32_t>(mPipelineInfo.mShaderStages.size());
    mPipelineInfo.mPipelineInfo.pStages = mPipelineInfo.mShaderStages.data();
    mPipelineInfo.mPipelineInfo.pVertexInputState = &mPipelineInfo.mVertexInputStateInfo;
    mPipelineInfo.mPipelineInfo.pInputAssemblyState = &mPipelineInfo.mInputAssemblyInfo;
    mPipelineInfo.mPipelineInfo.pDynamicState = &mPipelineInfo.mDynamicStateInfo;
    mPipelineInfo.mPipelineInfo.pViewportState = &mPipelineInfo.mViewportStateInfo;
    mPipelineInfo.mPipelineInfo.pRasterizationState = &mPipelineInfo.mRasterizationStateInfo;
    mPipelineInfo.mPipelineInfo.pMultisampleState = &mPipelineInfo.mMultisampleStateInfo;
    mPipelineInfo.mPipelineInfo.pColorBlendState = &mPipelineInfo.mColorBlendStateInfo;
    mPipelineInfo.mPipelineInfo.pDepthStencilState = &mPipelineInfo.mDepthStencilStateInfo;
    mPipelineInfo.mPipelineInfo.renderPass = renderPass;
    mPipelineInfo.mPipelineInfo.subpass = 0;
    mPipelineInfo.mPipelineInfo.basePipelineHandle = nullptr; // Optional
    mPipelineInfo.mPipelineInfo.basePipelineIndex = -1; // Optional
}
