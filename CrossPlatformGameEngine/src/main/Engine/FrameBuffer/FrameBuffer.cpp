#include "FrameBuffer.h"

#include "../RenderPass/RenderPass.h"
#include "../RenderTarget/RenderTarget.h"

#include <stdexcept>

using namespace ascen;

FrameBuffer::FrameBuffer(
    const VkDevice device,
    const RenderPassPtr& renderPass,
    const RenderTargetPtr& renderTarget,
    const VkExtent2D& extent) :
    mDevice(device),
    mRenderPass(renderPass->handle()),
    mImageViews(renderTarget->getImageViews())
{
    resize(extent.width, extent.height);
}

void FrameBuffer::destroy(const VkDevice device)
{
    vkDestroyFramebuffer(device, mHandle, nullptr);
}

void FrameBuffer::resize(const uint32_t width = 0, const uint32_t height = 0)
{
    if (width != 0 && height != 0)
    {
        destroy(mDevice);

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = mRenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(mImageViews.size());
        framebufferInfo.pAttachments = mImageViews.data();
        framebufferInfo.width = width;
        framebufferInfo.height = height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(mDevice, &framebufferInfo, nullptr, &mHandle) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}
