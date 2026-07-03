#include "FrameBufferFactory.h"

#include "FrameBuffer.h"

using namespace ascen;

FrameBufferFactory::FrameBufferFactory(const VkDevice device) : mDevice(device) {}

FrameBufferPtr FrameBufferFactory::create(
    const RenderPassPtr& renderPass,
    const RenderTargetPtr& renderTarget,
    const VkExtent2D extent) const
{
    return std::make_shared<FrameBuffer>(mDevice, renderPass, renderTarget, extent);
}
