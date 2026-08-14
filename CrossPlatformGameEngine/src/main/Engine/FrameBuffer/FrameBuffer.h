#pragma once

#include "../Handle/Handle.h"
#include "../Core/Types.h"

#include <vulkan/vulkan.h>

namespace ascen
{

    class FrameBuffer : public Handle<VkFramebuffer>
    {
    public:
        FrameBuffer(
        const VkDevice device,
        const RenderPassPtr& renderPass,
        const RenderTargetPtr& renderTarget,
        const VkExtent2D& extent);

        void destroy(const VkDevice device) override;

        void resize(const uint32_t width, const uint32_t height);

    private:
        const VkDevice mDevice;
        const VkRenderPass mRenderPass;
        const std::vector<VkImageView> mImageViews;
        uint32_t mWidth = 0;
        uint32_t mHeight = 0;
    };

}