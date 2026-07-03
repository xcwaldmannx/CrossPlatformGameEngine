#pragma once

#include "../../Core/Types.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    class RenderPassFactory
    {
    public:
        RenderPassFactory(VkDevice device);

        RenderPassPtr create(
            const std::vector<Attachment>& attachments,
            const std::vector<SubPass>& subPasses,
            const std::vector<SubPassDependency>& subPassDependencies) const;

    private:
        VkDevice mDevice;
    };

}
