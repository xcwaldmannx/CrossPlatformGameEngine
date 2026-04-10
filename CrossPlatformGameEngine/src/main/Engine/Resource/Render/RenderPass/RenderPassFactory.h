#pragma once

#include "../../../Core/Types.h"
#include "RenderPass.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    class RenderPassFactory
    {
    public:
        RenderPassFactory(const VkDevice device);

        RenderPassPtr create(
            const std::vector<RenderPass::Attachment>& attachments,
            const std::vector<RenderPass::SubPass>& subPasses,
            const std::vector<RenderPass::SubPassDependency>& subPassDependencies) const;

    private:
        const VkDevice mDevice;
    };

}
