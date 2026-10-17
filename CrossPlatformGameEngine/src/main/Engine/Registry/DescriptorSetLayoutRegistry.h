#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../Descriptor/DescriptorFactory.h"

namespace ascen
{

    class DescriptorSetLayoutRegistry : public Registry_I<registry::DescriptorSetLayoutEntry, DescriptorSetLayoutPtr>
    {
    public:
        DescriptorSetLayoutRegistry(
            const VkDevice device,
            const DescriptorFactory& descriptorFactory) :
            mDevice(device),
            mDescriptorFactory(descriptorFactory) {}

        void reconstruct(const registry::DescriptorSetLayoutEntry& entry, DescriptorSetLayoutPtr& resource) override
        {
            if (resource) resource->destroy(mDevice);

            std::vector<VkDescriptorSetLayoutBinding> bindings;

            std::vector<uint32_t> bindFlags;

            for (const auto& binding : entry.mBindings)
            {
                bindings.push_back(mDescriptorFactory.createBinding(
                    binding.mSlot.mSlot,
                    static_cast<VkDescriptorType>(binding.mSlot.mType),
                    binding.mCount,
                    binding.mStage));

                bindFlags.push_back(binding.mBindFlags);
            }

            resource = mDescriptorFactory.createSetLayout(bindings, bindFlags);
        }

    private:
        const VkDevice mDevice;
        const DescriptorFactory& mDescriptorFactory;
    };

}
