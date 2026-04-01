#include "ComputeGpuFramePass.h"

using namespace ascen;

ComputeGpuFramePass::ComputeGpuFramePass(
    const FramePassType type,
    const std::string pipeline,
    const std::vector<std::string> descriptorSets,
    const std::vector<GpuResource> resources,
    const std::array<uint32_t, 3> groups) :
    GpuFramePass(type, pipeline, descriptorSets, resources),
    mGroups(groups) {}