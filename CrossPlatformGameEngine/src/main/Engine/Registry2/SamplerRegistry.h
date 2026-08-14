#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../Resource/Sampler/SamplerFactory.h"
#include "../Resource/Sampler/Sampler.h"

namespace ascen
{

    class SamplerRegistry : public Registry_I<registry::SamplerEntry, SamplerPtr>
    {
    public:
        SamplerRegistry(
            const VkDevice device,
            const SamplerFactory& samplerFactory) :
            mDevice(device),
            mSamplerFactory(samplerFactory) {}

        void reconstruct(const registry::SamplerEntry &entry, SamplerPtr& resource) override
        {
            if (resource) resource->destroy(mDevice);

            resource = mSamplerFactory.create();
        }

    private:
        const VkDevice mDevice;
        const SamplerFactory& mSamplerFactory;
    };

}
