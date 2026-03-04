#pragma once

namespace ascen
{
    enum class FramePassType
    {
        NONE,
        CPU,
        GRAPHICS,
        COMPUTE,
    };

    struct FramePass
    {
        FramePass(const FramePassType type);

        const FramePassType mType;
    };

}