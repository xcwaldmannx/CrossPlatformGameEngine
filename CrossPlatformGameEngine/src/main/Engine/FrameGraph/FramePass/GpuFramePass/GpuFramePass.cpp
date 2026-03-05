#include "GpuFramePass.h"

using namespace ascen;

GpuFramePass::GpuFramePass(
    const FramePassType type,
    const std::vector<VkDescriptorSet> descriptorSets,
    const VkPipeline pipeline,
    const VkPipelineLayout pipelineLayout,
    const std::vector<std::string> readBuffers,
    const std::vector<std::string> writeBuffers,
    const std::vector<VkImageView> readTextures,
    const std::vector<VkImageView> writeTextures) :
    FramePass(type),
    mDescriptorSets(descriptorSets),
    mPipeline(pipeline),
    mPipelineLayout(pipelineLayout),
    mReadBuffers(readBuffers),
    mWriteBuffers(writeBuffers),
    mReadTextures(readTextures),
    mWriteTextures(writeTextures) {}