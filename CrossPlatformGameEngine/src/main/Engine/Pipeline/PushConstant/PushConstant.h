#pragma once

#include "../../Core/RenderContext.h"

namespace ascen
{

    class PushConstant
    {
    public:
        PushConstant(
            VkPipelineLayout layout,
            uint32_t offset,
            uint32_t size,
            VkShaderStageFlags stageFlags);

        void upload(
            VkCommandBuffer commandBuffer,
            const void* data) const;

        uint32_t getOffset() const;
        uint32_t getSize() const;

    private:
        VkPushConstantRange mPushConstantRange{};
        VkPipelineLayout mLayout;
        uint32_t mOffset;
        uint32_t mSize;
        VkShaderStageFlags mStageFlags;
    };

}
