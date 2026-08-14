#include "LineCommandRecorder.h"

#include "../../Pipeline/GraphicsPipeline/GraphicsPipeline.h"

#include <vector>

using namespace ascen;

void LineCommandRecorder::record(
    const VkCommandBuffer commandBuffer,
    const GraphicsPipelinePtr& pipeline,
    const std::vector<VkDescriptorSet> descriptorSets,
    const std::vector<VkBuffer> vertexBuffers,
    const VkBuffer indexBuffer)
{

    const auto& pipelineLayout = pipeline->getLayout();

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->handle());

    pipeline->uploadPushConstants(commandBuffer);

    const std::vector<VkDeviceSize> vertexOffsets(vertexBuffers.size(), 0);
    vkCmdBindVertexBuffers(
        commandBuffer,
        0,
        vertexBuffers.size(),
        vertexBuffers.data(),
        vertexOffsets.data());

    vkCmdBindIndexBuffer(
        commandBuffer,
        indexBuffer,
        0,
        VK_INDEX_TYPE_UINT32);

    // TODO: create dynamic offsets for dynamic buffers. This is currently hard-coded
    std::vector<uint32_t> dynamicOffets =
    {
        // frameIndex * 64 * 2 // camera UBO, 2 mat4s, 64 bytes each
    };

    if (!descriptorSets.empty())
    {
        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            descriptorSets.size(),
            descriptorSets.data(),
            static_cast<uint32_t>(dynamicOffets.size()),
            nullptr); //&dynamicOffets[0]);
    }

    vkCmdDrawIndexed(commandBuffer, 10'000, 1, 0, 0, 0);
}
