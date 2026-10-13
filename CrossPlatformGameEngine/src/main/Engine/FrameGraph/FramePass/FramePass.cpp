#include "FramePass.h"

using namespace ascen;



FramePass::FramePass(
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
            const uint32_t computeGroups[3],
            const std::unordered_map<uint32_t, transfer::Transfer> transfers):
    mType(type),
    mDescriptorSetIds(descriptorSetIds),
    mPipelineId(pipelineId),
    mDrawMode(drawMode),
    mRenderPassId(renderPassId),
    mRenderTargetId(renderTargetId),
    mFrameBufferId(frameBufferId),
    mExtent(extent),
    mVertexBufferIds(vertexBufferIds),
    mIndexBufferId(indexBufferId),
    mIndirectBufferId(indirectBufferId),
    mComputeGroups { computeGroups[0], computeGroups[1], computeGroups[2] },
    mTransfers(transfers) {}

void FramePass::destroy(const VkDevice device) {}

void FramePass::updateTransfer(
    const uint32_t id,
    const std::variant<transfer::BufferRegion, transfer::ImageRegion, transfer::BufferImageRegion>& region)
{
    if (const auto it = mTransfers.find(id); it != mTransfers.end())
    {
        auto& transfer = it->second;
        transfer.mRegion = region;
    }
    else
    {
        throw std::runtime_error("Transfer does not exist.");
    }
}
