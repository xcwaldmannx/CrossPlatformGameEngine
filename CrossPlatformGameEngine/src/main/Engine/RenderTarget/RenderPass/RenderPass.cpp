#include "RenderPass.h"

#include <stdexcept>

using namespace ascen;

RenderPass::RenderPass(
    const VkDevice device,
    const std::vector<Attachment>& attachments,
    const std::vector<SubPass>& subPasses,
    const std::vector<SubPassDependency>& subPassDependencies)
{
    createAttachments(attachments);
    createSubPassDescriptions(subPasses);
    createSubPassDependencies(subPassDependencies);

    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = static_cast<uint32_t>(mAttachments.size());
    createInfo.pAttachments = mAttachments.data();
    createInfo.subpassCount = mSubPassDescriptions.size();
    createInfo.pSubpasses = mSubPassDescriptions.data();
    createInfo.dependencyCount = mSubPassDependencies.size();
    createInfo.pDependencies = mSubPassDependencies.data();

    if (vkCreateRenderPass(device, &createInfo, nullptr, &mHandle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

void RenderPass::destroy(const VkDevice device)
{
    vkDestroyRenderPass(device, mHandle, nullptr);
}

void RenderPass::createAttachments(const std::vector<Attachment>& attachments)
{
    mAttachments.reserve(attachments.size());

    for (const auto& attachment : attachments)
    {
        VkAttachmentDescription desc{};
        desc.format         = static_cast<VkFormat>(attachment.mFormat);
        desc.samples        = VK_SAMPLE_COUNT_1_BIT;
        desc.loadOp         = static_cast<VkAttachmentLoadOp>(attachment.mLoadOp);
        desc.storeOp        = static_cast<VkAttachmentStoreOp>(attachment.mStoreOp);
        desc.stencilLoadOp  = static_cast<VkAttachmentLoadOp>(attachment.mDepthStencilLoadOp);
        desc.stencilStoreOp = static_cast<VkAttachmentStoreOp>(attachment.mDepthStencilStoreOp);
        desc.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;

        switch (attachment.mType)
        {
            case ATTACHMENT_PRESENT:
                desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                break;
            case ATTACHMENT_COLOR:
                desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                break;
            case ATTACHMENT_DEPTH:
                desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                break;
        }

        mAttachments.push_back(desc);
    }
}

void RenderPass::createSubPassDescriptions(const std::vector<SubPass>& subPasses)
{
    mSubPassDescriptions.reserve(subPasses.size());
    mInputAttachmentReferences.resize(subPasses.size());
    mColorAttachmentReferences.resize(subPasses.size());
    mDepthAttachmentReferences.resize(subPasses.size());

    for (unsigned int i = 0; i < subPasses.size(); i++)
    {
        const auto& subPass = subPasses.at(i);

        for (auto idx : subPass.mInputAttachmentIndices)
        {
            VkAttachmentReference ref{};
            ref.attachment = idx;
            ref.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            mInputAttachmentReferences[i].push_back(ref);
        }

        for (auto idx : subPass.mColorAttachmentIndices)
        {
            VkAttachmentReference ref{};
            ref.attachment = idx;
            ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            mColorAttachmentReferences[i].push_back(ref);
        }

        VkAttachmentReference* depthPtr = nullptr;

        if (subPass.mDepthAttachmentIndex >= 0)
        {
            VkAttachmentReference depthRef{};
            depthRef.attachment = subPass.mDepthAttachmentIndex;
            depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            mDepthAttachmentReferences[i] = depthRef;
            depthPtr = &mDepthAttachmentReferences[i];
        }

        VkSubpassDescription desc{};
        desc.pipelineBindPoint = static_cast<VkPipelineBindPoint>(subPass.mBindPoint);
        desc.inputAttachmentCount = static_cast<uint32_t>(mInputAttachmentReferences[i].size());
        desc.pInputAttachments = mInputAttachmentReferences[i].empty() ? nullptr : mInputAttachmentReferences[i].data();
        desc.colorAttachmentCount = static_cast<uint32_t>(mColorAttachmentReferences[i].size());
        desc.pColorAttachments = mColorAttachmentReferences[i].empty() ? nullptr : mColorAttachmentReferences[i].data();
        desc.pDepthStencilAttachment = depthPtr;

        mSubPassDescriptions.push_back(desc);
    }
}

void RenderPass::createSubPassDependencies(const std::vector<SubPassDependency>& subPassDependencies)
{
    mSubPassDependencies.reserve(subPassDependencies.size());

    for (const auto& subPassDependency : subPassDependencies)
    {
        VkSubpassDependency desc{};
        desc.srcSubpass = subPassDependency.mSrcSubpass;
        desc.dstSubpass = subPassDependency.mDstSubpass;
        desc.srcStageMask = subPassDependency.mSrcStageMask;
        desc.dstStageMask = subPassDependency.mDstStageMask;
        desc.srcAccessMask = subPassDependency.mSrcAccessMask;
        desc.dstAccessMask = subPassDependency.mDstAccessMask;

        mSubPassDependencies.push_back(desc);
    }
}
