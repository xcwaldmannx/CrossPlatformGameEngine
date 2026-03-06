#pragma once

#include "../GpuFramePass.h"

namespace ascen
{

    enum class GraphicsMode
    {
        MESH,
        LINES,
    };

    struct GraphicsGpuFramePass : GpuFramePass
    {
        GraphicsGpuFramePass(
            const FramePassType type,
            const GraphicsMode mode,
            const std::string pipeline,
            const std::vector<std::string> descriptorSets,
            const std::vector<GpuResource> resources);

        const GraphicsMode mMode;
    };

}