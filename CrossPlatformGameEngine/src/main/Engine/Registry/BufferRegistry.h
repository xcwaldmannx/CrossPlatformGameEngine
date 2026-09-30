#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../Resource/Buffer/BufferFactory.h"

namespace ascen
{

    class BufferRegistry : public Registry_I<registry::BufferEntry, BufferPtr>
    {
    public:
        BufferRegistry(
            const VkDevice device,
            const BufferFactory& bufferFactory) :
            mDevice(device),
            mBufferFactory(bufferFactory) {}

        void reconstruct(const registry::BufferEntry& entry, BufferPtr& resource) override
        {
            if (resource) resource->destroy(mDevice);

            resource = mBufferFactory.create(
                entry.mCapacity,
                entry.mStride,
                entry.mUsageFlags,
                entry.mMemoryFlags);
        }

    private:
        const VkDevice mDevice;
        const BufferFactory& mBufferFactory;
    };

}
