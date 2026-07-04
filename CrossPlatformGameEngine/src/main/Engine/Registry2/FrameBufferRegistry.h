#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../FrameBuffer/FrameBufferFactory.h"
#include "../FrameBuffer/FrameBuffer.h"

#include <any>

namespace ascen
{

    class FrameBufferRegistry : public Registry_I<registry::FrameBufferEntry, FrameBufferPtr>
    {
    public:
        FrameBufferRegistry(
            const VkDevice device,
            const FrameBufferFactory& frameBufferFactory,
            std::unordered_map<uint64_t, registry::Resource>* idToResource) :
            mDevice(device),
            mFrameBufferFactory(frameBufferFactory),
            mIdToResource(idToResource) {}

        void reconstruct(const registry::FrameBufferEntry &entry, std::shared_ptr<FrameBuffer> &resource) override
        {
            if (resource) resource->destroy(mDevice);

            const VkExtent2D extent(entry.mWidth, entry.mHeight);

            const RenderPassPtr& renderPass = std::any_cast<RenderPassPtr>(mIdToResource->at(entry.mRenderPassId));
            const RenderTargetPtr& renderTarget = std::any_cast<RenderTargetPtr>(mIdToResource->at(entry.mRenderTargetId));

            resource = mFrameBufferFactory.create(renderPass, renderTarget, extent);
        }

    private:
        const VkDevice mDevice;
        const FrameBufferFactory& mFrameBufferFactory;
        std::unordered_map<uint64_t, registry::Resource>* mIdToResource = nullptr;
    };

}
