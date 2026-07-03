#pragma once

#include "../../Core/Values.h"
#include "../../Core/Types.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    class RenderTargetFactory
    {
    public:
        RenderTargetFactory(VkDevice device);

        RenderTargetPtr create(
            const Format format,
            const std::vector<VkImage>& images) const;

    private:
        VkDevice mDevice;
    };

}
