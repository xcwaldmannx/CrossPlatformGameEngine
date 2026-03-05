#pragma once

namespace ascen
{
    enum class FramePassType
    {
        NONE,
        CPU,
        GRAPHICS,
        COMPUTE,
        TRANSFER,
    };

    struct FramePass
    {
        FramePass(const FramePassType type);

        const FramePassType mType;
    };

}