#include "RenderTargetFactory.h"

#include "RenderTarget.h"

using namespace ascen;

RenderTargetFactory::RenderTargetFactory(VkDevice device) : mDevice(device) {}

RenderTargetPtr RenderTargetFactory::create(
    const Format format,
    const std::vector<VkImage>& images) const
{
    return std::make_shared<RenderTarget>(mDevice, format, images);
}
