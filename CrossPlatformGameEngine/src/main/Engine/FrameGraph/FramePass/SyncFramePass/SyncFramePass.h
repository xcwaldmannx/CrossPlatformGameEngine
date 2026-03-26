#pragma once

#include <vector>

#include "../FramePass.h"
#include "../GpuFramePass/GpuFramePass.h"

namespace ascen
{
    struct GpuResource;

    struct SyncFramePass : FramePass
    {
        SyncFramePass(
            const FramePassType type,
            const VkBuffer buffer,
            const VkAccessFlags2 srcAccess,
            const VkPipelineStageFlags2 srcStage,
            const VkAccessFlags2 dstAccess,
            const VkPipelineStageFlags2 dstStage);

        const VkBuffer mBuffer;
        const VkAccessFlags2 mSrcAccess;
        const VkPipelineStageFlags2 mSrcStage;
        const VkAccessFlags2 mDstAccess;
        const VkPipelineStageFlags2 mDstStage;
    };

}