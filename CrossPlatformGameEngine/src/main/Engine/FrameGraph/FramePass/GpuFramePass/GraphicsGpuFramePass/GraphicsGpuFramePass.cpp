#include "GraphicsGpuFramePass.h"

using namespace ascen;

GraphicsGpuFramePass::GraphicsGpuFramePass(
    const FramePassType type,
    const GraphicsMode mode,
    const std::string& pipeline,
    const std::vector<std::string>& descriptorSets,
    const std::vector<GpuResource>& resources) :
    GpuFramePass(type, pipeline, descriptorSets, resources),
    mMode(mode) {}