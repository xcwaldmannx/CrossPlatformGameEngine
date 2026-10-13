#include "PushConstantFactory.h"

#include "PushConstant.h"

using namespace ascen;

PushConstantFactory::PushConstantFactory() {}

PushConstantPtr PushConstantFactory::create(
    VkPipelineLayout layout,
    uint32_t offset,
    uint32_t size,
    VkShaderStageFlags stageFlags) const
{
    return std::make_shared<PushConstant>(layout, offset, size, stageFlags);
}
