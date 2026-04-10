#pragma once

#include "../../../Core/Types.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    class RenderTargetFactory
    {
    public:
        RenderTargetFactory(const VkDevice device);

        RenderTargetPtr create(
            const VkFormat imageFormat,
            const std::vector<VkImage>& images) const;

    private:
        const VkDevice mDevice;
    };

}
