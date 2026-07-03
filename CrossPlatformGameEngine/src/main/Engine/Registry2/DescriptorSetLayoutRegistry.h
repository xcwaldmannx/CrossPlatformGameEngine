#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../Descriptor/DescriptorFactory.h"

namespace ascen
{

    class DescriptorSetLayoutRegistry : public Registry_I<registry::DescriptorSetLayoutEntry, DescriptorSetLayoutPtr>
    {
    public:
        DescriptorSetLayoutRegistry(const DescriptorFactory& descriptorFactory) : mDescriptorFactory(descriptorFactory) {}

        void reconstruct(const registry::DescriptorSetLayoutEntry& entry, DescriptorSetLayoutPtr& resource) override
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings;

            for (const auto& binding : entry.mBindings)
            {
                bindings.push_back(mDescriptorFactory.createBinding(
                    binding.mLocation.mSlot,
                    static_cast<VkDescriptorType>(binding.mLocation.mType),
                    binding.mStage));
            }

            resource = mDescriptorFactory.createSetLayout(bindings);
        }

    private:
        const DescriptorFactory& mDescriptorFactory;
    };

}
