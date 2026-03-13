#pragma once

#include "../FramePass.h"

namespace ascen
{

    struct SyncFramePass : FramePass
    {
        SyncFramePass(const FramePassType type);
    };

}