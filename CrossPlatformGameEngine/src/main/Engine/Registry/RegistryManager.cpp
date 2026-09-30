#include "RegistryManager.h"

#include <any>

#include "../Core/VulkanContext.h"

using namespace ascen;

RegistryManager::RegistryManager(
    const VulkanContext& vulkanContext,
    const RenderContext& renderContext) :
    mPhysicalDevice(vulkanContext.getPhysicalDevice()),
    mDevice(vulkanContext.getDevice()),
    mRenderContext(renderContext),
    mGraphicsQueue(vulkanContext.getGraphicsQueue()),
    mCommandPool(renderContext.getCommandPool())
{
    const VkDevice device = vulkanContext.getDevice();

    mRegistries.emplace_back(VertexRegistry{});
    mRegistries.emplace_back(BufferRegistry{ device, vulkanContext.getBufferFactory() });
    mRegistries.emplace_back(TextureRegistry{ device, renderContext.getCommandPool(), vulkanContext.getTextureFactory() });
    mRegistries.emplace_back(SamplerRegistry{ device, vulkanContext.getSamplerFactory() });
    mRegistries.emplace_back(DescriptorPoolRegistry{ device, vulkanContext.getDescriptorFactory() });
    mRegistries.emplace_back(DescriptorSetLayoutRegistry{ device, vulkanContext.getDescriptorFactory() });
    mRegistries.emplace_back(DescriptorSetRegistry{ device, vulkanContext.getDescriptorFactory(), &mIdToResource });
    mRegistries.emplace_back(RenderPassRegistry{ device, vulkanContext.getRenderPassFactory() });
    mRegistries.emplace_back(RenderTargetRegistry{ device, vulkanContext.getRenderTargetFactory(), &mIdToResource });
    mRegistries.emplace_back(FrameBufferRegistry{ device, vulkanContext.getFrameBufferFactory(), &mIdToResource });
    mRegistries.emplace_back(GraphicsPipelineRegistry{ device, renderContext.getSwapchain(), vulkanContext.getGraphicsPipelineFactory(), &mIdToResource });
    mRegistries.emplace_back(ComputePipelineRegistry{ device, vulkanContext.getComputePipelineFactory(), &mIdToResource });
    mRegistries.emplace_back(FramePassRegistry{});

};

void RegistryManager::reconstruct()
{
    for (auto& registryVariant : mRegistries)
    {
        std::visit([this](auto& concreteRegistry)
        {
            using ConcreteRegistryType = std::decay_t<decltype(concreteRegistry)>;
            using CurrentEntryType = ConcreteRegistryType::EntryType;
            using CurrentResourceType = ConcreteRegistryType::ResourceType;

            const std::type_index typeId = std::type_index(typeid(CurrentResourceType));

            const auto it = mTypeToIds.find(typeId);
            if (it == mTypeToIds.end()) return;

            const std::vector<uint64_t>& ids = it->second;

            for (const uint64_t id : ids)
            {
                const std::shared_ptr<CurrentEntryType>& storedEntry = std::any_cast<std::shared_ptr<CurrentEntryType>>(mIdToEntry.at(id));

                std::shared_ptr<Handle_I>& genericResource = mIdToResource[id];
                CurrentResourceType& concreteResource = reinterpret_cast<CurrentResourceType&>(genericResource);

                concreteRegistry.reconstruct(*storedEntry, concreteResource);
            }

        }, registryVariant);
    }
}

void RegistryManager::deconstruct()
{
    // free descriptor sets while their pools still exist
    for (auto& [id, resource] : mIdToResource)
    {
        if (auto set = std::dynamic_pointer_cast<DescriptorSet>(resource)) set->destroy(mDevice);
    }

    // destroy all other resources
    for (auto& [id, resource] : mIdToResource)
    {
        if (!resource) continue;
        resource->destroy(mDevice);
    }
}

void RegistryManager::uploadBuffer(
    const std::string& name,
    const void* items,
    const uint32_t itemCount,
    const uint32_t itemSize,
    const uint32_t offset) const
{
    const uint64_t id = mHasher(name);
    uploadBuffer(id, items, itemCount, itemSize, offset);
}

void RegistryManager::uploadBuffer(
    const uint64_t id,
    const void* items,
    const uint32_t itemCount,
    const uint32_t itemSize,
    const uint32_t offset) const
{
    if (mIdToResource.contains(id))
    {
        const BufferPtr& resource = std::dynamic_pointer_cast<Buffer>(mIdToResource.at(id));
        resource->upload(mPhysicalDevice, mDevice, mGraphicsQueue, mCommandPool, items, itemCount, itemSize, offset);
    }
    else
    {
        throw std::runtime_error("Buffer does not exist!");
    }
}

void RegistryManager::resizeBuffer(const std::string& name, const uint32_t itemCount)
{
    const uint64_t id = mHasher(name);
    resizeBuffer(id, itemCount);
}

void RegistryManager::resizeBuffer(const uint64_t id, const uint32_t itemCount)
{
    if (mIdToEntry.contains(id))
    {
        vkDeviceWaitIdle(mDevice);

        const auto entryIt = mIdToEntry.find(id);
        auto& entry = *std::any_cast<std::shared_ptr<registry::BufferEntry>&>(entryIt->second);
        entry.mCapacity = itemCount;

        if (mIdToResource.contains(id))
        {
            auto bufferIt = mIdToResource.find(id);
            BufferPtr buffer = std::dynamic_pointer_cast<Buffer>(bufferIt->second);

            for (auto& registryVariant : mRegistries)
            {
                if (auto* registry = std::get_if<BufferRegistry>(&registryVariant))
                {
                    registry->reconstruct(entry, buffer);
                    break;
                }
            }

            bufferIt->second = buffer;

            const auto setsIt = mTypeToIds.find(typeid(DescriptorSetPtr));

            if (setsIt != mTypeToIds.end())
            {
                for (uint64_t setId : setsIt->second)
                {
                    const auto& setEntry = *std::any_cast<const std::shared_ptr<registry::DescriptorSetEntry>&>(mIdToEntry.at(setId));

                    for (const auto& resource : setEntry.mResources)
                    {
                        if (resource.mResourceId != id) continue;

                        auto set = std::dynamic_pointer_cast<DescriptorSet>(mIdToResource.at(setId));

                        set->updateBuffer(
                            mDevice,
                            resource.mLocation.mSlot,
                            static_cast<VkDescriptorType>(resource.mLocation.mType),
                            buffer->handle(),
                            resource.mSize);
                    }
                }
            }
        }
    }
}

void RegistryManager::uploadTexture(const std::string& name, const std::vector<unsigned char>& pixels) const
{
    const uint64_t id = mHasher(name);
    uploadTexture(id, pixels);
}

void RegistryManager::uploadTexture(const uint64_t id, const std::vector<unsigned char>& pixels) const
{
    if (mIdToResource.contains(id))
    {
        const TexturePtr& resource = std::dynamic_pointer_cast<Texture>(mIdToResource.at(id));
        resource->update(mPhysicalDevice, mDevice, mGraphicsQueue, mCommandPool, pixels);
    }
    else
    {
        throw std::runtime_error("Texture does not exist!");
    }
}

void RegistryManager::resizeTexture(const std::string& name, const dim::Extent2D extent)
{
    const uint64_t id = mHasher(name);
    resizeTexture(id, extent);
}

void RegistryManager::resizeTexture(const uint64_t id, const dim::Extent2D extent)
{
    if (mIdToEntry.contains(id))
    {
        vkDeviceWaitIdle(mDevice);

        const auto entryIt = mIdToEntry.find(id);
        auto& entry = *std::any_cast<std::shared_ptr<registry::TextureEntry>&>(entryIt->second);

        switch (extent.mMode)
        {
            case dim::Extent2D::Mode::SWAPCHAIN:
                entry.mExtent.mExtent.width = mRenderContext.getSwapchain()->getExtent().width;
                entry.mExtent.mExtent.height = mRenderContext.getSwapchain()->getExtent().height;
                break;
            case dim::Extent2D::Mode::FIXED:
                entry.mExtent = extent;
                break;
        }

        if (mIdToResource.contains(id))
        {
            auto textureIt = mIdToResource.find(id);
            TexturePtr texture = std::dynamic_pointer_cast<Texture>(textureIt->second);

            for (auto& registryVariant : mRegistries)
            {
                if (auto* registry = std::get_if<TextureRegistry>(&registryVariant))
                {
                    registry->reconstruct(entry, texture);
                    break;
                }
            }

            textureIt->second = texture;

            const auto setsIt = mTypeToIds.find(typeid(DescriptorSetPtr));

            if (setsIt != mTypeToIds.end())
            {
                for (uint64_t setId : setsIt->second)
                {
                    const auto& setEntry = *std::any_cast<const std::shared_ptr<registry::DescriptorSetEntry>&>(mIdToEntry.at(setId));

                    for (const auto& resource : setEntry.mResources)
                    {
                        if (resource.mResourceId != id) continue;

                        auto set = std::dynamic_pointer_cast<DescriptorSet>(mIdToResource.at(setId));

                        set->updateTexture(
                            mDevice,
                            resource.mLocation.mSlot,
                            static_cast<VkDescriptorType>(resource.mLocation.mType),
                            texture->handle());
                    }
                }
            }
        }
    }
}

void RegistryManager::resizeSwapchainDependentResources()
{
    vkDeviceWaitIdle(mDevice);

    // framebuffers
    auto framebuffersIt = mTypeToIds.find(typeid(FrameBufferPtr));

    if (framebuffersIt != mTypeToIds.end())
    {
        for (uint64_t framebufferId : framebuffersIt->second)
        {
            auto& framebufferEntry = *std::any_cast<std::shared_ptr<registry::FrameBufferEntry>>(mIdToEntry.at(framebufferId));

            if (framebufferEntry.mExtent.mMode == dim::Extent2D::Mode::SWAPCHAIN)
            {
                auto framebufferIt = mIdToResource.find(framebufferId);

                if (framebufferIt != mIdToResource.end())
                {
                    FrameBufferPtr framebufferResource = std::dynamic_pointer_cast<FrameBuffer>(framebufferIt->second);

                    for (auto& registryVariant : mRegistries)
                    {
                        if (auto* registry = std::get_if<FrameBufferRegistry>(&registryVariant))
                        {
                            registry->reconstruct(framebufferEntry, framebufferResource);
                            break;
                        }
                    }

                    framebufferIt->second = framebufferResource;
                }
            }
        }
    }

    // textures
    auto texturesIt = mTypeToIds.find(typeid(TexturePtr));

    if (texturesIt != mTypeToIds.end())
    {
        for (uint64_t textureId : texturesIt->second)
        {
            auto& textureEntry = *std::any_cast<std::shared_ptr<registry::TextureEntry>>(mIdToEntry.at(textureId));

            if (textureEntry.mExtent.mMode == dim::Extent2D::Mode::SWAPCHAIN)
            {
                auto textureIt = mIdToResource.find(textureId);

                if (textureIt != mIdToResource.end())
                {
                    TexturePtr textureResource = std::dynamic_pointer_cast<Texture>(textureIt->second);

                    for (auto& registryVariant : mRegistries)
                    {
                        if (auto* registry = std::get_if<TextureRegistry>(&registryVariant))
                        {
                            registry->reconstruct(textureEntry, textureResource);
                            break;
                        }
                    }

                    textureIt->second = textureResource;

                    // descriptor sets
                    const auto setsIt = mTypeToIds.find(typeid(DescriptorSetPtr));

                    if (setsIt != mTypeToIds.end())
                    {
                        for (uint64_t setId : setsIt->second)
                        {
                            auto setEntry = *std::any_cast<const std::shared_ptr<registry::DescriptorSetEntry>&>(mIdToEntry.at(setId));

                            for (const auto& resource : setEntry.mResources)
                            {
                                if (resource.mResourceId != textureId) continue;

                                auto set = std::dynamic_pointer_cast<DescriptorSet>(mIdToResource.at(setId));

                                set->updateTexture(
                                    mDevice,
                                    resource.mLocation.mSlot,
                                    static_cast<VkDescriptorType>(resource.mLocation.mType),
                                    textureResource->handle());
                            }
                        }
                    }

                    // render targets
                    const auto targetsIt = mTypeToIds.find(typeid(RenderTargetPtr));

                    if (targetsIt != mTypeToIds.end())
                    {
                        for (uint64_t targetId : targetsIt->second)
                        {
                            auto targetEntry = *std::any_cast<const std::shared_ptr<registry::RenderTargetEntry>&>(mIdToEntry.at(targetId));

                            for (const auto& targetTextureId : targetEntry.mTextureIds)
                            {
                                if (textureId == targetTextureId)
                                {
                                    auto targetIt = mIdToResource.find(targetId);

                                    if (targetIt != mIdToResource.end())
                                    {
                                        auto targetResource = std::dynamic_pointer_cast<RenderTarget>(targetIt->second);

                                        for (auto& registryVariant : mRegistries)
                                        {
                                            if (auto* registry = std::get_if<RenderTargetRegistry>(&registryVariant))
                                            {
                                                registry->reconstruct(targetEntry, targetResource);
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void RegistryManager::updateTransfer(
    const std::string& name,
    const uint32_t transferId,
    const std::variant<transfer::BufferRegion, transfer::ImageRegion, transfer::BufferImageRegion>& region)
{
    const uint64_t id = mHasher(name);
    updateTransfer(id, transferId, region);
}

void RegistryManager::updateTransfer(
    const uint64_t id,
    const uint32_t transferId,
    const std::variant<transfer::BufferRegion, transfer::ImageRegion, transfer::BufferImageRegion>& region) const
{
    if (mIdToResource.contains(id))
    {
        const FramePassPtr& resource = std::dynamic_pointer_cast<FramePass>(mIdToResource.at(id));
        resource->updateTransfer(transferId, region);
    }
}

void RegistryManager::uploadPushConstants(const VkCommandBuffer commandBuffer) const
{
    const auto graphicsType = std::type_index(typeid(GraphicsPipelinePtr));
    const std::vector<uint64_t>& graphicsIds = mTypeToIds.at(graphicsType);
    for (const auto id : graphicsIds)
    {
        const auto& resource = std::dynamic_pointer_cast<GraphicsPipeline>(mIdToResource.at(id));
        resource->uploadPushConstants(commandBuffer);
    }

    const auto computeType = std::type_index(typeid(ComputePipelinePtr));
    const std::vector<uint64_t>& computeIds = mTypeToIds.at(computeType);
    for (const auto id : computeIds)
    {
        const auto& resource = std::dynamic_pointer_cast<ComputePipeline>(mIdToResource.at(id));
        resource->uploadPushConstants(commandBuffer);
    }
}
