#pragma once

#include "../Core/Types.h"
#include "Registry_I.h"
#include "../Descriptor/DescriptorFactory.h"

#include "../Resource/Texture/Texture.h"
#include "../Resource/Sampler/Sampler.h"
#include "../Resource/Buffer/Buffer.h"

#include <any>
#include <unordered_map>

namespace ascen
{
    class VulkanContext;

    class DescriptorSetRegistry : public Registry_I<registry::DescriptorSetEntry, DescriptorSetPtr>
    {
    public:
        DescriptorSetRegistry(
            const VkDevice device,
            const DescriptorFactory& descriptorFactory,
            std::unordered_map<uint64_t, std::shared_ptr<Handle_I>>* idToResource) :
            mDevice(device),
            mDescriptorFactory(descriptorFactory),
            mIdToResource(idToResource) {}

        void reconstruct(const registry::DescriptorSetEntry& entry, DescriptorSetPtr& resource) override
        {
            if (resource) resource->destroy(mDevice);

            std::vector<DescriptorSet::Write> writes;

            for (const auto& r : entry.mResources)
            {
                switch (r.mLocation.mType)
                {
                    case DESCRIPTOR_TYPE_IMAGE:
                    {
                        const auto& texture = std::dynamic_pointer_cast<Texture>(mIdToResource->at(r.mResourceId));

                        writes.push_back(mDescriptorFactory.createImageWrite(
                            static_cast<VkDescriptorType>(r.mLocation.mType),
                            texture->handle(),
                            VK_NULL_HANDLE,
                            r.mLocation.mSlot));
                        break;
                    }
                    case DESCRIPTOR_TYPE_SAMPLER:
                    {
                        const auto& sampler = std::dynamic_pointer_cast<Sampler>(mIdToResource->at(r.mResourceId));

                        writes.push_back(mDescriptorFactory.createImageWrite(
                            static_cast<VkDescriptorType>(r.mLocation.mType),
                            VK_NULL_HANDLE,
                            sampler->handle(),
                            r.mLocation.mSlot));
                        break;
                    }
                    case DESCRIPTOR_TYPE_IMAGE_SAMPLER:
                        break;
                    case DESCRIPTOR_TYPE_UBO:
                    case DESCRIPTOR_TYPE_UBO_DYNAMIC:
                    case DESCRIPTOR_TYPE_SSBO:
                    case DESCRIPTOR_TYPE_SSBO_DYNAMIC:
                    {
                        const auto& buffer = std::dynamic_pointer_cast<Buffer>(mIdToResource->at(r.mResourceId));

                        writes.push_back(mDescriptorFactory.createBufferWrite(
                            static_cast<VkDescriptorType>(r.mLocation.mType),
                            buffer->handle(),
                            0,
                            r.mSize,
                            r.mLocation.mSlot));
                    }
                    default:
                        break;
                }
            }

            const auto& descriptorPool = std::dynamic_pointer_cast<DescriptorPool>(mIdToResource->at(entry.mPoolId));
            const auto& descriptorSetLayout = std::dynamic_pointer_cast<DescriptorSetLayout>(mIdToResource->at(entry.mLayoutId));

            resource = mDescriptorFactory.createSet(descriptorPool, descriptorSetLayout, writes);
        }

    private:
        const VkDevice mDevice;
        const DescriptorFactory& mDescriptorFactory;
        std::unordered_map<uint64_t, std::shared_ptr<Handle_I>>* mIdToResource = nullptr;
    };

}
