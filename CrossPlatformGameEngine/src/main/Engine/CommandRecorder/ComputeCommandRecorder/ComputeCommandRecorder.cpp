#include "ComputeCommandRecorder.h"

#include "../../Pipeline/ComputePipeline/ComputePipeline_I.h"

using namespace ascen;

ComputeCommandRecorder::ComputeCommandRecorder(
    PipelineRegistry& pipelineRegistry,
    DescriptorRegistry& descriptorRegistry,
    ResourceRegistry& resourceRegistry) :
    CommandRecorder_I(pipelineRegistry, descriptorRegistry, resourceRegistry) {}

void ComputeCommandRecorder::record(
    VkCommandBuffer commandBuffer,
    const ComputeGpuFramePass* pass,
    uint32_t frameIndex)
{
    const auto& pipeline = PipelineRegistryBackend::getComputePipeline(mPipelineRegistry, pass->mPipeline);
    const auto& pipelineLayout = pipeline->getLayout();

    std::vector<VkDescriptorSet> descriptorSets;
    for (const auto& descriptorSet : pass->mDescriptorSets)
    {
        const auto& descriptorSetPtr = DescriptorRegistryBackend::getDescriptorSet(mDescriptorRegistry, descriptorSet);
        descriptorSets.push_back(descriptorSetPtr->handle());
    }

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->handle());

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

    vkCmdDispatch(commandBuffer,
        pass->mGroups[0], pass->mGroups[1], pass->mGroups[2]);
}
