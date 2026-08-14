#include "RenderPassFactory.h"
#include "RenderPass.h"

using namespace ascen;

RenderPassFactory::RenderPassFactory(VkDevice device) : mDevice(device) {}

RenderPassPtr RenderPassFactory::create(
    const std::vector<renderpass::Attachment>& attachments,
    const std::vector<renderpass::SubPass>& subPasses,
    const std::vector<renderpass::SubPassDependency>& subPassDependencies) const
{
    return std::make_shared<RenderPass>(mDevice, attachments, subPasses, subPassDependencies);
}
