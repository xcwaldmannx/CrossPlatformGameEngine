#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../FrameGraph/FramePass/FramePass.h"

namespace ascen
{

    class FramePassRegistry : public Registry_I<registry::FramePassEntry, FramePassPtr>
    {
    public:
        void reconstruct(const registry::FramePassEntry &entry, std::shared_ptr<FramePass> &resource) override
        {
            resource = std::make_shared<FramePass>(
                entry.mType,
                entry.mDescriptorSetIds,
                entry.mPipelineId,
                entry.mGraphicsParams.mDrawMode,
                entry.mGraphicsParams.mRenderPassId,
                entry.mGraphicsParams.mRenderTargetId,
                entry.mGraphicsParams.mFrameBufferId,
                entry.mGraphicsParams.mExtent,
                entry.mGraphicsParams.mVertexBufferIds,
                entry.mGraphicsParams.mIndexBufferId,
                entry.mGraphicsParams.mIndirectBufferId,
                entry.mComputeParams.mGroups,
                entry.mTransfers);
        }
    };

}
