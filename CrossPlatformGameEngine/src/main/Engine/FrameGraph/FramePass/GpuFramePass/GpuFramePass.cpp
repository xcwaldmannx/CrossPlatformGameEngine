#include "GpuFramePass.h"

using namespace ascen;

GpuFramePass::GpuFramePass(
    const FramePassType type,
    const std::string& pipeline,
    const std::string& renderTarget,
    const std::vector<std::string>& descriptorSets,
    const std::vector<GpuResource>& resources) :
    FramePass(type),
    mPipeline(pipeline),
    mRenderTarget(renderTarget),
    mDescriptorSets(descriptorSets),
    mResources(resources) {}
