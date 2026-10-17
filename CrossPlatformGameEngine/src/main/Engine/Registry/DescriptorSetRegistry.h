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
                switch (r.mSlot.mType)
                {
                    case DESCRIPTOR_TYPE_IMAGE:
                    {
                        if (const auto* textureId = std::get_if<uint64_t>(&r.mResourceIds))
                        {
                            if (*textureId == 0) break;

                            const auto& texture = std::dynamic_pointer_cast<Texture>(mIdToResource->at(*textureId));

                            writes.push_back(mDescriptorFactory.createImageWrite(
                                static_cast<VkDescriptorType>(r.mSlot.mType),
                                texture->handle(),
                                VK_NULL_HANDLE,
                                r.mSlot.mSlot));
                        }
                        else if (const auto* textureIds = std::get_if<std::vector<uint64_t>>(&r.mResourceIds))
                        {
                            if (!textureIds->empty())
                            {
                                std::vector<VkDescriptorImageInfo> infos;
                                infos.reserve(textureIds->size());

                                for (const auto id : *textureIds)
                                {
                                    if (id == 0) continue;

                                    const auto& texture = std::dynamic_pointer_cast<Texture>(mIdToResource->at(id));

                                    VkDescriptorImageInfo info{};
                                    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                    info.imageView = texture->handle();
                                    info.sampler = VK_NULL_HANDLE;

                                    infos.push_back(info);
                                }

                                DescriptorSet::Write write{};
                                write.mWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                write.mWrite.dstBinding = r.mSlot.mSlot;
                                write.mWrite.dstArrayElement = 0;
                                write.mWrite.descriptorType = static_cast<VkDescriptorType>(r.mSlot.mType);
                                write.mWrite.descriptorCount = infos.size();
                                write.mInfo = infos;

                                writes.push_back(write);
                            }
                        }
                        break;
                    }
                    case DESCRIPTOR_TYPE_SAMPLER:
                    {
                        if (const auto* samplerId = std::get_if<uint64_t>(&r.mResourceIds))
                        {
                            const auto& sampler = std::dynamic_pointer_cast<Sampler>(mIdToResource->at(*samplerId));

                            writes.push_back(mDescriptorFactory.createImageWrite(
                                static_cast<VkDescriptorType>(r.mSlot.mType),
                                VK_NULL_HANDLE,
                                sampler->handle(),
                                r.mSlot.mSlot));
                        }
                        break;
                    }
                    case DESCRIPTOR_TYPE_IMAGE_SAMPLER:
                        break;
                    case DESCRIPTOR_TYPE_UBO:
                    case DESCRIPTOR_TYPE_UBO_DYNAMIC:
                    case DESCRIPTOR_TYPE_SSBO:
                    case DESCRIPTOR_TYPE_SSBO_DYNAMIC:
                    {
                        if (const auto* bufferId = std::get_if<uint64_t>(&r.mResourceIds))
                        {
                            const auto& buffer = std::dynamic_pointer_cast<Buffer>(mIdToResource->at(*bufferId));

                            writes.push_back(mDescriptorFactory.createBufferWrite(
                                static_cast<VkDescriptorType>(r.mSlot.mType),
                                buffer->handle(),
                                0,
                                r.mSize,
                                r.mSlot.mSlot));
                        }
                        break;
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
