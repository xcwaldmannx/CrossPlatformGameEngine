#pragma once

#include "../../Handle/Handle.h"
#include "../../Core/Types.h"

#include <vector>

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

    private:
        const VkExtent2D& mExtent;
    };

}