#pragma once

#include <string>

#include "../FramePass.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    enum class ResourceUsage
    {
        BUFFER_VERTEX,
        BUFFER_INDEX,
        BUFFER_UNIFORM,
        BUFFER_STORAGE,
        BUFFER_INDIRECT,
        BUFFER_TRANSFER_SRC,
        BUFFER_TRANSFER_DST,

        IMAGE_SAMPLED,
        IMAGE_STORAGE,
        IMAGE_COLOR_ATTACH,
        IMAGE_DEPTH_ATTACH,
        IMAGE_TRANSFER_SRC,
        IMAGE_TRANSFER_DST,
        IMAGE_PRESENT
    };

    enum class ResourceAccess
    {
        READ       = 0x00,
        WRITE      = 0x01,
        READ_WRITE = 0x02
    };

    enum class ResourceStage
    {
        VERTEX,
        FRAGMENT,
        COMPUTE,
    };

    struct GpuResource
    {
        std::string mName;
        ResourceUsage mUsage;
        ResourceAccess mAccess;
        ResourceStage mStage;
    };

    struct GpuFramePass : FramePass
    {
        GpuFramePass(
            const FramePassType type,
            const std::string& pipeline,
            const std::string& renderTarget,
            const std::vector<std::string>& descriptorSets,
            const std::vector<GpuResource>& resources);

        const std::string mPipeline;
        const std::string mRenderTarget;
        const std::vector<std::string> mDescriptorSets;
        const std::vector<GpuResource> mResources;
    };

}
