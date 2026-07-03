#include "RenderPassFactory.h"

#include "RenderPass.h"

using namespace ascen;

RenderPassFactory::RenderPassFactory(VkDevice device) : mDevice(device) {}

RenderPassPtr RenderPassFactory::create(
    const std::vector<Attachment>& attachments,
    const std::vector<SubPass>& subPasses,
    const std::vector<SubPassDependency>& subPassDependencies) const
{
    return std::make_shared<RenderPass>(mDevice, attachments, subPasses, subPassDependencies);
}
