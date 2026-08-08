#include "RegistryManager.h"

#include <any>

#include "../Core/VulkanContext.h"

using namespace ascen;

RegistryManager::RegistryManager(
    const VulkanContext& vulkanContext,
    const RenderContext& renderContext) :
    mPhysicalDevice(vulkanContext.getPhysicalDevice()),
    mDevice(vulkanContext.getDevice()),
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
    for (const auto& [id, resource] : mIdToResource)
    {
        if (resource) resource->destroy(mDevice);
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
