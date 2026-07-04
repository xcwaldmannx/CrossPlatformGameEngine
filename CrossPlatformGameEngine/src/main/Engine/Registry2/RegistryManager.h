#pragma once

#include "../Core/VulkanContext.h"
#include "../Core/RenderContext.h"

#include "Registry_I.h"
#include "VertexRegistry.h"
#include "DescriptorPoolRegistry.h"
#include "DescriptorSetLayoutRegistry.h"
#include "DescriptorSetRegistry.h"
#include "BufferRegistry.h"
#include "TextureRegistry.h"
#include "SamplerRegistry.h"
#include "RenderPassRegistry.h"
#include "RenderTargetRegistry.h"
#include "FrameBufferRegistry.h"
#include "GraphicsPipelineRegistry.h"
#include "ComputePipelineRegistry.h"

#include <cstdint>
#include <functional>
#include <string>
#include <typeindex>
#include <variant>

namespace ascen
{

    using RegistryTypes = std::variant<
        VertexRegistry,
        DescriptorPoolRegistry,
        DescriptorSetLayoutRegistry,
        DescriptorSetRegistry,
        BufferRegistry,
        TextureRegistry,
        SamplerRegistry,
        RenderPassRegistry,
        RenderTargetRegistry,
        FrameBufferRegistry,
        GraphicsPipelineRegistry,
        ComputePipelineRegistry>;

    template <typename T>
    struct RegistryTraits;

    template <typename Entry, typename Resource>
    struct RegistryTraits<Registry_I<Entry, Resource>>
    {
        using EntryType = Entry;
        using ResourceType = Resource;
    };

    class RegistryManager
    {
    public:
        RegistryManager(
            const VulkanContext& vulkanContext,
            const RenderContext& renderContext);

        template<Derived<registry::Entry> E>
        uint64_t registerResource(const E& entry)
        {
            const uint64_t id = mHasher(entry.mName);
            mIdToEntry.emplace(id, std::make_shared<E>(entry));

            const std::type_index typeId = std::type_index(typeid(E));
            mTypeToIds[typeId].push_back(id);

            return id;
        }

        void reconstruct();

        /*
        template<typename T>
        std::span<T> getMappedBuffer(const std::string& name)
        {
            auto it = mBuffers.find(name);
            if (it == mBuffers.end())
            {
                throw std::runtime_error("Mapped buffer does not exist!");
            }

            const BufferPtr& buf = it->second;

            void* mapped = buf->getMappedMemory();

            auto* typed = reinterpret_cast<T*>(static_cast<uint8_t*>(mapped));

            return std::span<T>(typed, buf->getItemCount());
        }
        */

        void uploadBuffer(const std::string& name, const void* items, uint32_t itemCount, uint32_t itemSize, uint32_t offset) const;
        void uploadBuffer(const uint64_t id, const void* items, uint32_t itemCount, uint32_t itemSize, uint32_t offset) const;

        void uploadTexture(const std::string& name, const std::vector<unsigned char>& pixels) const;
        void uploadTexture(const uint64_t id, const std::vector<unsigned char>& pixels) const;

        void uploadGraphicsPushConstant(const VkCommandBuffer commandBuffer, const uint64_t id, const uint32_t pushConstantId, const void* data) const;
        void uploadComputePushConstant(const VkCommandBuffer commandBuffer, const uint64_t id, const uint32_t pushConstantId, const void* data) const;

    private:
        const VkPhysicalDevice mPhysicalDevice;
        const VkDevice mDevice;
        const VkQueue mGraphicsQueue;
        const CommandPoolPtr& mCommandPool;

        std::vector<RegistryTypes> mRegistries;

        std::hash<std::string> mHasher;

        std::unordered_map<uint64_t, std::shared_ptr<registry::Entry>> mIdToEntry;
        std::unordered_map<std::type_index, std::vector<uint64_t>> mTypeToIds;
        std::unordered_map<uint64_t, registry::Resource> mIdToResource;
    };

}
