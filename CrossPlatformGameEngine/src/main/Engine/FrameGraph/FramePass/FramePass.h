#pragma once

#include <functional>
#include <ostream>
#include <string>

namespace ascen
{
    enum class FramePassType
    {
        NONE,
        CPU,
        GRAPHICS,
        COMPUTE,
        SYNC,
        TRANSFER,
    };

    struct FramePass
    {
        FramePass(
            const FramePassType type);

        const std::string getStringType() const;

        const FramePassType mType;
    };

}