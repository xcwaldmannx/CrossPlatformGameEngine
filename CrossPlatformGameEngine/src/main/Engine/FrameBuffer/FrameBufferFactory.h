#pragma once

#include "../Core/Types.h"

#include <vulkan/vulkan.h>

namespace ascen
{

    class FrameBufferFactory
    {
    public:
        FrameBufferFactory(const VkDevice device);

        FrameBufferPtr create(
            const RenderPassPtr& renderPass,
            const RenderTargetPtr& renderTarget,
            const VkExtent2D extent) const;

    private:
        VkDevice mDevice = VK_NULL_HANDLE;
    };

}
