#include "MeshCommandRecorder.h"

#include "../../FrameGraph/Graphics/GraphicsFramePass.h"

#include <vector>

using namespace ascen;

void MeshCommandRecorder::record(
    VkCommandBuffer commandBuffer,
    const GraphicsFramePass* framePass,
    uint32_t frameIndex,
    VkBuffer indirectBuffer,
    uint32_t drawCommandCount)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, framePass->mPipeline);

    std::vector<VkDeviceSize> vertexOffsets(framePass->mVertexBuffers.size(), 0);
    vkCmdBindVertexBuffers(
        commandBuffer,
        0,
        framePass->mVertexBuffers.size(),
        framePass->mVertexBuffers.data(),
        vertexOffsets.data());

    vkCmdBindIndexBuffer(
        commandBuffer,
        framePass->mIndexBuffer,
        0,
        VK_INDEX_TYPE_UINT32);

    // TODO: create dynamic offsets for dynamic buffers. This is currently hard-coded
    std::vector<uint32_t> dynamicOffets =
    {
        frameIndex * 64 * 2 // camera UBO, 2 mat4s, 64 bytes each
    };

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        framePass->mPipelineLayout,
        0,
        framePass->mDescriptorSets.size(),
        framePass->mDescriptorSets.data(),
        static_cast<uint32_t>(dynamicOffets.size()),
        &dynamicOffets[0]);

    vkCmdDrawIndexedIndirect(
        commandBuffer,
        indirectBuffer,
        0,
        drawCommandCount,
        sizeof(VkDrawIndexedIndirectCommand));
}
