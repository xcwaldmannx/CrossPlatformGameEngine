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
        const dim::Extent2D extent);

        void destroy(const VkDevice device) override;

        void resize(const uint32_t width, const uint32_t height);

    private:
        const VkDevice mDevice;
        const VkRenderPass mRenderPass;
        const std::vector<VkImageView> mImageViews;
        dim::Extent2D mExtent;
    };

}