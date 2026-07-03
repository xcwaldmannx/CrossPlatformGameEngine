#include "RegistryManager.h"

#include <any>

#include "../Core/VulkanContext.h"

using namespace ascen;

RegistryManager::RegistryManager(
    const VulkanContext& vulkanContext,
    const RenderContext& renderContext)
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
