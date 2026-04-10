#pragma once

#include <vector>

#include "../../../Handle/Handle.h"

#include <vulkan/vulkan.h>

namespace ascen
{

    class RenderPass : public Handle<VkRenderPass>
    {
    public:
        enum AttachmentType
        {
            PRESENT,
            COLOR,
            DEPTH
        };

        enum AttachmentFormat
        {
            ATTACHMENT_FORMAT_RGBA8_SRGB = VK_FORMAT_R8G8B8A8_SRGB,
        };

        enum AttachmentStoreOp
        {
            ATTACHMENT_STORE_OP_STORE = VK_ATTACHMENT_STORE_OP_STORE,
            ATTACHMENT_STORE_OP_NA = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            ATTACHMENT_STORE_OP_NONE = VK_ATTACHMENT_STORE_OP_NONE,
        };

        enum AttachmentLoadOp
        {
            ATTACHMENT_LOAD_OP_LOAD = VK_ATTACHMENT_LOAD_OP_LOAD,
            ATTACHMENT_LOAD_OP_CLEAR = VK_ATTACHMENT_LOAD_OP_CLEAR,
            ATTACHMENT_LOAD_OP_NA = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        };

        enum SubPassBindPoint
        {
            SUBPASS_BIND_POINT_GRAPHICS = VK_PIPELINE_BIND_POINT_GRAPHICS,
            SUBPASS_BIND_POINT_COMPUTE = VK_PIPELINE_BIND_POINT_COMPUTE,
        };

        enum SubPassPipelineStage
        {
            SUBPASS_PIPELINE_STAGE_DRAW_INDIRECT                  = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
            SUBPASS_PIPELINE_STAGE_VERTEX_INPUT                   = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
            SUBPASS_PIPELINE_STAGE_VERTEX_SHADER                  = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
            SUBPASS_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER    = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT,
            SUBPASS_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT,
            SUBPASS_PIPELINE_STAGE_GEOMETRY_SHADER                = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT,
            SUBPASS_PIPELINE_STAGE_FRAGMENT_SHADER                = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            SUBPASS_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS           = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            SUBPASS_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT        = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            SUBPASS_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            SUBPASS_PIPELINE_STAGE_COMPUTE_SHADER                 = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            SUBPASS_PIPELINE_STAGE_TRANSFER                       = VK_PIPELINE_STAGE_TRANSFER_BIT,
        };

        enum SubbPassAccessMask
        {
            SUBPASS_ACCESS_INDIRECT_COMMAND_READ          = VK_ACCESS_INDIRECT_COMMAND_READ_BIT,
            SUBPASS_ACCESS_INDEX_READ                     = VK_ACCESS_INDEX_READ_BIT,
            SUBPASS_ACCESS_VERTEX_ATTRIBUTE_READ          = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
            SUBPASS_ACCESS_UNIFORM_READ                   = VK_ACCESS_UNIFORM_READ_BIT,
            SUBPASS_ACCESS_INPUT_ATTACHMENT_READ          = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
            SUBPASS_ACCESS_SHADER_READ                    = VK_ACCESS_SHADER_READ_BIT,
            SUBPASS_ACCESS_SHADER_WRITE                   = VK_ACCESS_SHADER_WRITE_BIT,
            SUBPASS_ACCESS_COLOR_ATTACHMENT_READ          = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
            SUBPASS_ACCESS_COLOR_ATTACHMENT_WRITE         = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            SUBPASS_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ  = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
            SUBPASS_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            SUBPASS_ACCESS_TRANSFER_READ                  = VK_ACCESS_TRANSFER_READ_BIT,
            SUBPASS_ACCESS_TRANSFER_WRITE                 = VK_ACCESS_TRANSFER_WRITE_BIT,
            SUBPASS_ACCESS_HOST_READ                      = VK_ACCESS_HOST_READ_BIT,
            SUBPASS_ACCESS_HOST_WRITE                     = VK_ACCESS_HOST_WRITE_BIT,
            SUBPASS_ACCESS_MEMORY_READ                    = VK_ACCESS_MEMORY_READ_BIT,
            SUBPASS_ACCESS_MEMORY_WRITE                   = VK_ACCESS_MEMORY_WRITE_BIT,
            SUBPASS_ACCESS_NONE                           = VK_ACCESS_NONE,
        };

        struct Attachment
        {
            AttachmentType mType;
            AttachmentFormat mFormat;
            AttachmentLoadOp mLoadOp;
            AttachmentStoreOp mStoreOp;
            AttachmentLoadOp mDepthStencilLoadOp;
            AttachmentStoreOp mDepthStencilStoreOp;
        };

        struct SubPassDependency
        {
            uint32_t mSrcSubpass;
            uint32_t mDstSubpass;
            SubPassPipelineStage mSrcStageMask;
            SubPassPipelineStage mDstStageMask;
            SubbPassAccessMask mSrcAccessMask;
            SubbPassAccessMask mDstAccessMask;
        };

        struct SubPass
        {
            SubPassBindPoint mBindPoint;
            std::vector<uint32_t> mColorAttachmentIndices;
            std::vector<uint32_t> mInputAttachmentIndices;
            int32_t mDepthAttachmentIndex;
        };

    public:
        RenderPass(
            const VkDevice device,
            const std::vector<Attachment>& attachments,
            const std::vector<SubPass>& subPasses,
            const std::vector<SubPassDependency>& subPassDependencies);

        void destroy(const VkDevice device) override;

    private:
        std::vector<VkAttachmentDescription> mAttachments;
        std::vector<std::vector<VkAttachmentReference>> mInputAttachmentReferences;
        std::vector<std::vector<VkAttachmentReference>> mColorAttachmentReferences;
        std::vector<VkAttachmentReference> mDepthAttachmentReferences;
        std::vector<VkSubpassDescription> mSubPassDescriptions;
        std::vector<VkSubpassDependency> mSubPassDependencies;
    };

}