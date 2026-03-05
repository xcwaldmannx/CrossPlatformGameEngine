#include "GraphicsGpuFramePass.h"

using namespace ascen;

GraphicsGpuFramePass::GraphicsGpuFramePass(
    const FramePassType type,
    const GraphicsMode mode,
    const std::vector<VkDescriptorSet> descriptorSets,
    const VkPipeline pipeline,
    const VkPipelineLayout pipelineLayout,
    const std::vector<std::string> readBuffers,
    const std::vector<std::string> writeBuffers,
    const std::vector<VkImageView> readTextures,
    const std::vector<VkImageView> writeTextures,
    const std::vector<VkBuffer> vertexBuffers,
    const VkBuffer indexBuffer) :
    GpuFramePass(type, descriptorSets, pipeline, pipelineLayout,
        readBuffers, writeBuffers, readTextures, writeTextures),
    mVertexBuffers(vertexBuffers),
    mIndexBuffer(indexBuffer),
    mMode(mode) {}