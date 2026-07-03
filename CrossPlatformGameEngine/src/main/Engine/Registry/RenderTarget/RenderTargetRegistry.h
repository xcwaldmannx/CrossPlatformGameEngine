#pragma once

#include "../Registry_I.h"
#include "../../Core/Types.h"
#include "../../RenderTarget/FrameBuffer/FrameBuffer.h"
#include "../../RenderTarget/RenderPass/RenderPass.h"
#include "../../RenderTarget/RenderTarget/RenderTarget.h"

#include <string>
#include <unordered_map>

namespace ascen
{
    class VulkanContext;
    class RenderContext;

    class FrameBufferFactory;
    class RenderPassFactory;
    class RenderTargetFactory;

    struct RenderPassEntry
    {
        std::string mName;
        std::vector<RenderPass::Attachment> mAttachments;
        std::vector<RenderPass::SubPass> mSubPasses;
        std::vector<RenderPass::SubPassDependency> mSubPassDependencies;
    };

    struct RenderTargetEntry
    {
        std::string mName;
        Format mFormat;
        std::vector<VkImage> mImages;
        uint32_t mImageCount;
    };

    class RenderTargetRegistry : public Registry_I
    {
    public:
        RenderTargetRegistry(
            const VulkanContext& vulkanContext,
            const RenderContext& renderContext);

        void registerRenderPass(const RenderPassEntry& entry);
        void registerRenderTarget(const RenderTargetEntry& entry);

        void reconstruct() override;

        void cleanup();

    private:
        bool passExists(const std::string& name) const;
        bool targetExists(const std::string& name) const;

        bool isRegistered(const std::string& name) const;

    private:
        const VkDevice mDevice;

        const RenderPassFactory& mRenderPassFactory;
        const RenderTargetFactory& mRenderTargetFactory;

        std::vector<std::string> mRegisteredNames;

        std::vector<RenderPassEntry> mRenderPassEntries;
        std::vector<RenderTargetEntry> mRenderTargetEntries;

        std::unordered_map<std::string, FrameBufferPtr> mFrameBuffers;
        std::unordered_map<std::string, RenderPassPtr> mRenderPasses;
        std::unordered_map<std::string, RenderTargetPtr> mRenderTargets;

        friend class RenderTargetRegistryBackend;
    };

}