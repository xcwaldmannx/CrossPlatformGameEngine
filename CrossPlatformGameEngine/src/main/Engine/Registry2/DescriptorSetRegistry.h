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
            std::unordered_map<uint64_t, registry::Resource>* idToResource) :
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
                    case IMAGE:
                    {
                        const auto& texture = std::any_cast<TexturePtr>(mIdToResource->at(r.mResourceId));

                        writes.push_back(mDescriptorFactory.createImageWrite(
                            static_cast<VkDescriptorType>(r.mLocation.mType),
                            texture->handle(),
                            VK_NULL_HANDLE,
                            r.mLocation.mSlot));
                        break;
                    }
                    case SAMPLER:
                    {
                        const auto& sampler = std::any_cast<SamplerPtr>(mIdToResource->at(r.mResourceId));

                        writes.push_back(mDescriptorFactory.createImageWrite(
                            static_cast<VkDescriptorType>(r.mLocation.mType),
                            VK_NULL_HANDLE,
                            sampler->handle(),
                            r.mLocation.mSlot));
                        break;
                    }
                    case IMAGE_SAMPLER:
                        break;
                    case UBO:
                    case UBO_DYNAMIC:
                    case SSBO:
                    case SSBO_DYNAMIC:
                    default:
                    {
                        const auto& buffer = std::any_cast<BufferPtr>(mIdToResource->at(r.mResourceId));

                        writes.push_back(mDescriptorFactory.createBufferWrite(
                            static_cast<VkDescriptorType>(r.mLocation.mType),
                            buffer->handle(),
                            0,
                            r.mSize,
                            r.mLocation.mSlot));
                    }
                }
            }

            const auto& descriptorPool = std::any_cast<DescriptorPoolPtr>(mIdToResource->at(entry.mPoolId));
            const auto& descriptorSetLayout = std::any_cast<DescriptorSetLayoutPtr>(mIdToResource->at(entry.mLayoutId));

            resource = mDescriptorFactory.createSet(descriptorPool, descriptorSetLayout, writes);
        }

    private:
        const VkDevice mDevice;
        const DescriptorFactory& mDescriptorFactory;
        std::unordered_map<uint64_t, registry::Resource>* mIdToResource = nullptr;
    };

}
