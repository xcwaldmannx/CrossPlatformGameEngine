#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../RenderTarget/RenderTargetFactory.h"
#include "../RenderTarget/RenderTarget.h"

namespace ascen
{

    class RenderTargetRegistry : public Registry_I<registry::RenderTargetEntry, RenderTargetPtr>
    {
    public:
        RenderTargetRegistry(
        const VkDevice device,
        const RenderTargetFactory& renderTargetFactory) :
        mDevice(device),
        mRenderTargetFactory(renderTargetFactory) {}

        void reconstruct(const registry::RenderTargetEntry& entry, RenderTargetPtr& resource) override
        {
            if (resource) resource->destroy(mDevice);

            resource = mRenderTargetFactory.create(entry.mFormat, entry.mImages);
        }

    private:
        const VkDevice mDevice;
        const RenderTargetFactory& mRenderTargetFactory;
    };

}
