#pragma once

#include <vulkan/vulkan.h>

namespace ascen
{

    struct RecorderContext
    {

    };

    class CommandRecorder_I
    {
    public:
        virtual ~CommandRecorder_I();

        virtual void record(const RecorderContext& context) = 0;
    };

}
