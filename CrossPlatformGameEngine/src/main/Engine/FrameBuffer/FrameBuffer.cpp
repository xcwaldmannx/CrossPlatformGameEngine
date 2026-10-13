#include "FrameBuffer.h"

#include "../RenderPass/RenderPass.h"
#include "../RenderTarget/RenderTarget.h"

#include <stdexcept>

using namespace ascen;

FrameBuffer::FrameBuffer(
    const VkDevice device,
    const RenderPassPtr& renderPass,
    const RenderTargetPtr& renderTarget,
    const dim::Extent2D extent) :
    mDevice(device)
{
    if (extent.mExtent.width != 0 && extent.mExtent.height != 0)
    {
        destroy(mDevice);

        const auto imageViews = renderTarget->getImageViews();

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass->handle();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
        framebufferInfo.pAttachments = imageViews.data();
        framebufferInfo.width = extent.mExtent.width;
        framebufferInfo.height = extent.mExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(mDevice, &framebufferInfo, nullptr, &mHandle) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create framebuffer.");
        }
    }
    else
    {
        // throw std::runtime_error("Failed to create framebuffer. Extent was zero.");
    }
}

void FrameBuffer::destroy(const VkDevice device)
{
    vkDestroyFramebuffer(device, mHandle, nullptr);
}
