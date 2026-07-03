#include "GraphicsGpuFramePass.h"

using namespace ascen;

GraphicsGpuFramePass::GraphicsGpuFramePass(
    const FramePassType type,
    const GraphicsMode mode,
    const std::string& pipeline,
    const std::string& renderPass,
    const std::string& renderTarget,
    const std::vector<std::string>& descriptorSets,
    const std::vector<GpuResource>& resources) :
    GpuFramePass(type, pipeline, renderTarget, descriptorSets, resources),
    mMode(mode),
    mRenderPass(renderPass) {}