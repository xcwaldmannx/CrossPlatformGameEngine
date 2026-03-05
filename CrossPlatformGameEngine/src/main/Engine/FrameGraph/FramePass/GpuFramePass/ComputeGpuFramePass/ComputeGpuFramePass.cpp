#include "ComputeGpuFramePass.h"

using namespace ascen;

ComputeGpuFramePass::ComputeGpuFramePass(
    const FramePassType type,
    const std::vector<VkDescriptorSet> descriptorSets,
    const VkPipeline pipeline,
    const VkPipelineLayout pipelineLayout,
    const std::vector<std::string> readBuffers,
    const std::vector<std::string> writeBuffers,
    const std::vector<VkImageView> readTextures,
    const std::vector<VkImageView> writeTextures,
    const std::array<uint32_t, 3> groups) :
    GpuFramePass(type, descriptorSets, pipeline, pipelineLayout,
        readBuffers, writeBuffers, readTextures, writeTextures),
    mGroups(groups) {}