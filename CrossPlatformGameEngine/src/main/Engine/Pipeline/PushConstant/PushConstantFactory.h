#pragma once

#include "../../Core/Types.h"
#include "../ComputePipeline/ComputePipeline.h"

namespace ascen
{

    class PushConstantFactory
    {
    public:
        PushConstantFactory();

        PushConstantPtr create(VkPipelineLayout layout, uint32_t offset, uint32_t size, VkShaderStageFlags stageFlags) const;
    };

}
