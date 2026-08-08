#include "RenderTargetFactory.h"

#include "RenderTarget.h"

using namespace ascen;

RenderTargetFactory::RenderTargetFactory(VkDevice device) : mDevice(device) {}

RenderTargetPtr RenderTargetFactory::create(const std::vector<TexturePtr>& textures) const
{
    return std::make_shared<RenderTarget>(mDevice, textures);
}
