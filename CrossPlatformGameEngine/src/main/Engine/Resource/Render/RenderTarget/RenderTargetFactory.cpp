#include "RenderTargetFactory.h"

#include "RenderTarget.h"

using namespace ascen;

RenderTargetFactory::RenderTargetFactory(const VkDevice device) : mDevice(device) {}

RenderTargetPtr RenderTargetFactory::create(
    const VkFormat imageFormat,
    const std::vector<VkImage>& images) const
{
    return std::make_shared<RenderTarget>(mDevice, imageFormat, images);
}
