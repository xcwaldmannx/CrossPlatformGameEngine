#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../Descriptor/DescriptorFactory.h"

namespace ascen
{

    class DescriptorPoolRegistry : public Registry_I<registry::DescriptorPoolEntry, DescriptorPoolPtr>
    {
    public:
        DescriptorPoolRegistry(
            const VkDevice device,
            const DescriptorFactory& descriptorFactory) :
            mDevice(device),
            mDescriptorFactory(descriptorFactory) {}

        void reconstruct(const registry::DescriptorPoolEntry& entry, DescriptorPoolPtr& resource) override
        {
            if (resource) resource->destroy(mDevice);

            std::vector<DescriptorPool::Size> sizes;

            for (auto& [type, count] : entry.mDescriptorTypeCounts)
            {
                if (count > 0)
                {
                    DescriptorPool::Size size{};
                    size.type = static_cast<VkDescriptorType>(type);
                    size.descriptorCount = count;
                    sizes.push_back(size);
                }
            }

            resource = mDescriptorFactory.createPool(sizes);
        }

    private:
        const VkDevice mDevice;
        const DescriptorFactory& mDescriptorFactory;
    };

}
