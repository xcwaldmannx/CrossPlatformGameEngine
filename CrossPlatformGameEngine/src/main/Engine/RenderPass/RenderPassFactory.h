#pragma once

#include "../Core/Types.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    class RenderPassFactory
    {
    public:
        RenderPassFactory(VkDevice device);

        RenderPassPtr create(
            const std::vector<renderpass::Attachment>& attachments,
            const std::vector<renderpass::SubPass>& subPasses,
            const std::vector<renderpass::SubPassDependency>& subPassDependencies) const;

    private:
        VkDevice mDevice;
    };

}
