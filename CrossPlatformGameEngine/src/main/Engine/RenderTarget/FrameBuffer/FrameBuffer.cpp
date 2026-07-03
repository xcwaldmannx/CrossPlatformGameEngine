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
    mExtent(extent)
{
    const auto& attachments = renderTarget->getImageViews();

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass->handle();
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(
        device, &framebufferInfo, nullptr, &mHandle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create framebuffer!");
    }
}

void FrameBuffer::destroy(const VkDevice device)
{
    vkDestroyFramebuffer(device, mHandle, nullptr);
}
