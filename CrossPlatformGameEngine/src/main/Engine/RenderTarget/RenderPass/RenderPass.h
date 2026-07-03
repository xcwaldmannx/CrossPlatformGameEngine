#pragma once

#include "../../Core/Types.h"
#include "../../Handle/Handle.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    class RenderPass : public Handle<VkRenderPass>
    {
    public:
        RenderPass(
            const VkDevice device,
            const std::vector<Attachment>& attachments,
            const std::vector<SubPass>& subPasses,
            const std::vector<SubPassDependency>& subPassDependencies);

        void destroy(const VkDevice device) override;

    private:
        void createAttachments(const std::vector<Attachment>& attachments);
        void createSubPassDescriptions(const std::vector<SubPass>& subPasses);
        void createSubPassDependencies(const std::vector<SubPassDependency>& subPassDependencies);

    private:
        std::vector<VkAttachmentDescription> mAttachments;
        std::vector<std::vector<VkAttachmentReference>> mInputAttachmentReferences;
        std::vector<std::vector<VkAttachmentReference>> mColorAttachmentReferences;
        std::vector<VkAttachmentReference> mDepthAttachmentReferences;
        std::vector<VkSubpassDescription> mSubPassDescriptions;
        std::vector<VkSubpassDependency> mSubPassDependencies;
    };

}