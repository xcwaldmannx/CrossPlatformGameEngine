#include "MeshCommandRecorder.h"

#include "../../FrameGraph/FramePass/GpuFramePass/GraphicsGpuFramePass/GraphicsGpuFramePass.h"

#include <vector>

using namespace ascen;

MeshCommandRecorder::MeshCommandRecorder(
    PipelineRegistry& pipelineRegistry,
    DescriptorRegistry& descriptorRegistry,
    ResourceRegistry& resourceRegistry) :
    CommandRecorder_I(pipelineRegistry, descriptorRegistry, resourceRegistry) {}

void MeshCommandRecorder::record(
    VkCommandBuffer commandBuffer,
    const GraphicsGpuFramePass* pass,
    uint32_t frameIndex)
{
    const auto& pipeline = PipelineRegistryBackend::getGraphicsPipeline(mPipelineRegistry, pass->mPipeline);
    const auto& pipelineLayout = pipeline->getLayout();

    std::vector<VkDescriptorSet> descriptorSets;
    for (const auto& descriptorSet : pass->mDescriptorSets)
    {
        const auto& descriptorSetPtr = DescriptorRegistryBackend::getDescriptorSet(mDescriptorRegistry, descriptorSet);
        descriptorSets.push_back(descriptorSetPtr->handle());
    }

    std::vector<VkBuffer> vertexBuffers;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkBuffer indirectBuffer = VK_NULL_HANDLE;
    uint32_t indirectCount = 0;

    for (const auto & resource : pass->mResources)
    {
        switch (resource.mUsage)
        {
            case ResourceUsage::BUFFER_VERTEX:
            {
                const auto& vertexBufferPtr = ResourceRegistryBackend::getBuffer(mResourceRegistry, resource.mName);
                vertexBuffers.push_back(vertexBufferPtr->handle());
                break;
            }
            case ResourceUsage::BUFFER_INDEX:
            {
                indexBuffer = ResourceRegistryBackend::getBuffer(mResourceRegistry, resource.mName)->handle();
                break;
            }
            case ResourceUsage::BUFFER_INDIRECT:
            {
                const auto& buf = ResourceRegistryBackend::getBuffer(mResourceRegistry, resource.mName);
                indirectBuffer = buf->handle();
                indirectCount = static_cast<uint32_t>(buf->getItemCount());
                break;
            }
            default:
                break;
        }
    }

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->handle());

    std::vector<VkDeviceSize> vertexOffsets(vertexBuffers.size(), 0);
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
        frameIndex * 64 * 2 // camera UBO, 2 mat4s, 64 bytes each
    };

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        descriptorSets.size(),
        descriptorSets.data(),
        static_cast<uint32_t>(dynamicOffets.size()),
        &dynamicOffets[0]);

    vkCmdDrawIndexedIndirect(
        commandBuffer,
        indirectBuffer,
        0,
        indirectCount,
        sizeof(VkDrawIndexedIndirectCommand));
}
