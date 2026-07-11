#pragma once

#include "../../Handle/Handle.h"
#include "../../Core/Values.h"

#include <functional>


namespace ascen
{

    struct FramePass : Handle<void*>
    {
        FramePass(
            const FramePassType type,
            const std::vector<uint64_t> descriptorSetIds,
            const uint64_t pipelineId,
            const FramePassDrawMode drawMode,
            const uint64_t renderPassId,
            const uint64_t renderTargetId,
            const uint64_t frameBufferId,
            const VkExtent2D extent,
            const std::vector<uint64_t> vertexBufferIds,
            const uint64_t indexBufferId,
            const uint64_t indirectBufferId,
            const uint32_t computeGroups[3]);

        void destroy(VkDevice device) override;

        FramePassType mType;
        std::vector<uint64_t> mDescriptorSetIds;
        uint64_t mPipelineId = 0;

        FramePassDrawMode mDrawMode;
        uint64_t mRenderPassId = 0;
        uint64_t mRenderTargetId = 0;
        uint64_t mFrameBufferId = 0;
        VkExtent2D mExtent = { 0, 0 };

        std::vector<uint64_t> mVertexBufferIds;
        uint64_t mIndexBufferId = 0;
        uint64_t mIndirectBufferId = 0;

        uint32_t mComputeGroups[3] = { 0, 0, 0 };
    };

}
