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

    mRegistries.push_back(VertexRegistry{});
    mRegistries.push_back(DescriptorPoolRegistry{ device, vulkanContext.getDescriptorFactory() });
    mRegistries.push_back(DescriptorSetLayoutRegistry{ device, vulkanContext.getDescriptorFactory() });
    mRegistries.push_back(DescriptorSetRegistry{ device, vulkanContext.getDescriptorFactory(), &mIdToResource });
    mRegistries.push_back(BufferRegistry{ device, vulkanContext.getBufferFactory() });
    mRegistries.push_back(TextureRegistry{ device, renderContext.getCommandPool(), vulkanContext.getTextureFactory() });
    mRegistries.push_back(SamplerRegistry{ device, vulkanContext.getSamplerFactory() });
    mRegistries.push_back(RenderPassRegistry{ device, vulkanContext.getRenderPassFactory() });
    mRegistries.push_back(RenderTargetRegistry{ device, vulkanContext.getRenderTargetFactory() });
    mRegistries.push_back(FrameBufferRegistry{ device, vulkanContext.getFrameBufferFactory(), &mIdToResource });
    mRegistries.push_back(GraphicsPipelineRegistry{ device, renderContext.getSwapchain(), renderContext.getRenderPass(), vulkanContext.getGraphicsPipelineFactory(), &mIdToResource });
    mRegistries.push_back(ComputePipelineRegistry{ device, vulkanContext.getComputePipelineFactory(), &mIdToResource });

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

            const std::type_index typeId = std::type_index(typeid(CurrentEntryType));

            auto it = mTypeToIds.find(typeId);
            if (it == mTypeToIds.end()) return;

            const std::vector<uint64_t>& ids = it->second;
            for (const uint64_t id : ids)
            {
                std::any storedEntry = mIdToEntry.at(id);
                std::shared_ptr<CurrentEntryType> entryPtr = std::any_cast<std::shared_ptr<CurrentEntryType>>(storedEntry);

                registry::Resource& genericResource = mIdToResource[id];
                CurrentResourceType& concreteResource = reinterpret_cast<CurrentResourceType&>(genericResource);

                concreteRegistry.reconstruct(*entryPtr, concreteResource);
            }

        }, registryVariant);
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
        const BufferPtr& resource = std::any_cast<BufferPtr>(mIdToResource.at(id));
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
        const TexturePtr& resource = std::any_cast<TexturePtr>(mIdToResource.at(id));
        resource->update(mPhysicalDevice, mDevice, mGraphicsQueue, mCommandPool, pixels);
    }
    else
    {
        throw std::runtime_error("Texture does not exist!");
    }
}

void RegistryManager::uploadGraphicsPushConstant(const VkCommandBuffer commandBuffer, const uint64_t id, const uint32_t pushConstantId, const void* data) const
{
    if (mIdToResource.contains(id))
    {
        const GraphicsPipelinePtr& resource = std::any_cast<GraphicsPipelinePtr>(mIdToResource.at(id));
        resource->uploadPushConstant(commandBuffer, pushConstantId, data);
    }
}

void RegistryManager::uploadComputePushConstant(const VkCommandBuffer commandBuffer, const uint64_t id, const uint32_t pushConstantId, const void* data) const
{
    if (mIdToResource.contains(id))
    {
        const ComputePipelinePtr& resource = std::any_cast<ComputePipelinePtr>(mIdToResource.at(id));
        resource->uploadPushConstant(commandBuffer, pushConstantId, data);
    }
}
