#include "RenderPassFactory.h"

using namespace ascen;

RenderPassFactory::RenderPassFactory(const VkDevice device) : mDevice(device) {}

RenderPassPtr RenderPassFactory::create(
    const std::vector<RenderPass::Attachment>& attachments,
    const std::vector<RenderPass::SubPass>& subPasses,
    const std::vector<RenderPass::SubPassDependency>& subPassDependencies) const
{
    return std::make_shared<RenderPass>(mDevice, attachments, subPasses, subPassDependencies);
}
