#include "SyncFramePass.h"

using namespace ascen;

SyncFramePass::SyncFramePass(
    const FramePassType type,
    const VkBuffer buffer,
    const VkAccessFlags2 srcAccess,
    const VkPipelineStageFlags2 srcStage,
    const VkAccessFlags2 dstAccess,
    const VkPipelineStageFlags2 dstStage)
    : FramePass(type),
      mBuffer(buffer),
      mSrcAccess(srcAccess),
      mSrcStage(srcStage),
      mDstAccess(dstAccess),
      mDstStage(dstStage) {}
