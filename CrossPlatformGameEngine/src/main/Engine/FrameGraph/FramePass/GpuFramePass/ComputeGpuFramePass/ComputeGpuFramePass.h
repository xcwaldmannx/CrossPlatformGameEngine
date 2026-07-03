#pragma once

#include "../GpuFramePass.h"

#include <array>
#include <functional>

namespace ascen
{

    struct ComputeGpuFramePass : GpuFramePass
    {
        ComputeGpuFramePass(
            const FramePassType type,
            const std::string& pipeline,
            const std::string& renderTarget,
            const std::vector<std::string>& descriptorSets,
            const std::vector<GpuResource>& resources,
            const std::array<uint32_t,3>& groups);

        const std::array<uint32_t, 3> mGroups;
    };

}