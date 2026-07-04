#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../RenderPass/RenderPassFactory.h"
#include "../RenderTarget/RenderPass/RenderPass.h"

namespace ascen
{

    class RenderPassRegistry : public Registry_I<registry::RenderPassEntry, RenderPassPtr>
    {
    public:
        RenderPassRegistry(
        const VkDevice device,
        const RenderPassFactory& renderPassFactory) :
        mDevice(device),
        mRenderPassFactory(renderPassFactory) {}

        void reconstruct(const registry::RenderPassEntry& entry, RenderPassPtr& resource) override
        {
            if (resource) resource->destroy(mDevice);

            resource = mRenderPassFactory.create(entry.mAttachments, entry.mSubPasses, entry.mSubPassDependencies);
        }

    private:
        const VkDevice mDevice;
        const RenderPassFactory& mRenderPassFactory;
    };

}
