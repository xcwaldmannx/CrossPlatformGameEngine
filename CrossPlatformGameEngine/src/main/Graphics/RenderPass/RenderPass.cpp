#include "RenderPass.h"

#include <stdexcept>

#include <array>

using namespace ascen;

RenderPass::RenderPass(
	VkPhysicalDevice physicalDevice,
	VkFormat colorFormat,
    VkFormat depthFormat)
{
    Attachment colorAttachment{};
    colorAttachment.mDesc.format         = colorFormat; // from swapchain
    colorAttachment.mDesc.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.mDesc.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.mDesc.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.mDesc.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.mDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.mDesc.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.mDesc.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    colorAttachment.mRef.attachment      = 0;
    colorAttachment.mRef.layout          = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    mAttachments.push_back(colorAttachment);

    Attachment depthAttachment{};
    depthAttachment.mDesc.format         = depthFormat;
    depthAttachment.mDesc.samples        = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.mDesc.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.mDesc.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.mDesc.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.mDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.mDesc.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.mDesc.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachment.mRef.attachment      = 1;
    depthAttachment.mRef.layout          = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    mAttachments.push_back(depthAttachment);

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &mAttachments[0].mRef;
    subpass.pDepthStencilAttachment = &mAttachments[1].mRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcAccessMask = 0;

    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment.mDesc, depthAttachment.mDesc };

    mRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    mRenderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    mRenderPassInfo.pAttachments = attachments.data();
    mRenderPassInfo.subpassCount = 1;
    mRenderPassInfo.pSubpasses = &subpass;
    mRenderPassInfo.dependencyCount = 1;
    mRenderPassInfo.pDependencies = &dependency;
}

void RenderPass::create(VkDevice device)
{
    if (vkCreateRenderPass(device, &mRenderPassInfo, nullptr, &mHandle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void RenderPass::destroy(VkDevice device)
{
    vkDestroyRenderPass(device, mHandle, nullptr);
}
