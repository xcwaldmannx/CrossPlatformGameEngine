#include "ComputeCommandRecorder.h"

#include <ostream>

#include "../../Pipeline/ComputePipeline/ComputePipeline_I.h"

using namespace ascen;

void ComputeCommandRecorder::record(
    VkCommandBuffer commandBuffer,
    const ComputePipelinePtr& pipeline,
    const std::vector<VkDescriptorSet>& descriptorSets,
    const uint32_t computeGroups[3])
{
    const auto& pipelineLayout = pipeline->getLayout();

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->handle());

    pipeline->uploadPushConstants(commandBuffer);

    // TODO: create dynamic offsets for dynamic buffers. This is currently hard-coded
    std::vector<uint32_t> dynamicOffets =
    {
        //frameIndex * 64 * 2 // camera UBO, 2 mat4s, 64 bytes each
    };

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        pipelineLayout,
        0,
        descriptorSets.size(),
        descriptorSets.data(),
        static_cast<uint32_t>(dynamicOffets.size()),
        nullptr); // &dynamicOffets[0]);

    vkCmdDispatch(commandBuffer, computeGroups[0], computeGroups[1], computeGroups[2]);
}
