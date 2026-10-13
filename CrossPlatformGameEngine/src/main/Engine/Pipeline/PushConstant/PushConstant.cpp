#include "PushConstant.h"

using namespace ascen;

PushConstant::PushConstant(
    VkPipelineLayout layout,
    uint32_t offset,
    uint32_t size,
    VkShaderStageFlags stageFlags) :
    mLayout(layout),
    mOffset(offset),
    mSize(size),
    mStageFlags(stageFlags) {}

void PushConstant::upload(
    VkCommandBuffer commandBuffer,
    const void* data) const
{
    vkCmdPushConstants(commandBuffer, mLayout, mStageFlags, mOffset, mSize, data);
}

uint32_t PushConstant::getOffset() const
{
    return mOffset;
}

uint32_t PushConstant::getSize() const
{
    return mSize;
}
