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
#include "FramePassRegistry.h"

#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <typeindex>
#include <variant>
#include <vector>

namespace ascen
{

    using RegistryTypes = std::variant<
        VertexRegistry,
        BufferRegistry,
        TextureRegistry,
        SamplerRegistry,
        DescriptorPoolRegistry,
        DescriptorSetLayoutRegistry,
        DescriptorSetRegistry,
        RenderPassRegistry,
        RenderTargetRegistry,
        FrameBufferRegistry,
        GraphicsPipelineRegistry,
        ComputePipelineRegistry,
        FramePassRegistry>;

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

            using ResourceType = E::ResourceType;
            mTypeToIds[typeid(ResourceType)].push_back(id);

            std::cout << "Registered resource type: " << typeid(ResourceType).name() << " with name " << entry.mName << std::endl;

            return id;
        }

        void reconstruct();

        void deconstruct();

        /* might need this later
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

        void updateTransfer(
            const std::string& name,
            const uint32_t transferId,
            const std::variant<transfer::BufferRegion, transfer::ImageRegion, transfer::BufferImageRegion>& region);
        void updateTransfer(
            const uint64_t id,
            const uint32_t transferId,
            const std::variant<transfer::BufferRegion, transfer::ImageRegion, transfer::BufferImageRegion>& region) const;

        template<typename T>
        void downloadTransfer(
            const std::string& name,
            const uint32_t transferId,
            T* data)
        {
            const uint64_t id = mHasher(name);
            downloadTransfer(id, transferId, data);
        }

        template<typename T>
        void downloadTransfer(
            const uint64_t id,
            const uint32_t transferId,
            T* data) const
        {
            if (mIdToResource.contains(id))
            {
                const FramePassPtr& resource = std::dynamic_pointer_cast<FramePass>(mIdToResource.at(id));
                auto& transfer = resource->mTransfers.at(transferId);
                if (transfer.mState == transfer::TRANSFER_STATE_READY)
                {
                    const BufferPtr& buffer = getResource<Buffer>(transfer.mDest);
                    buffer->read(mDevice, data, sizeof(T), 0);
                    transfer.mState = transfer::TRANSFER_STATE_IDLE;
                    std::cout << "IDLE" << std::endl;
                }
            }
        }

        template<typename T>
        void updatePushConstant(const std::string& name, const uint32_t pushConstantId, const T& data)
        {
            const uint64_t id = mHasher(name);
            updatePushConstant<T>(id, pushConstantId, data);
        }

        template<typename T>
        void updatePushConstant(const uint64_t id, const uint32_t pushConstantId, const T& data)
        {
            const std::shared_ptr<Pipeline_I>& pipeline = std::dynamic_pointer_cast<Pipeline_I>(mIdToResource.at(id));
            pipeline->updatePushConstant(pushConstantId, data);
        }

        void uploadPushConstants(const VkCommandBuffer commandBuffer) const;

        template<typename R>
        std::shared_ptr<R> getResource(const uint64_t id) const
        {
            return std::dynamic_pointer_cast<R>(mIdToResource.at(id));
        }

        template<typename R>
        std::vector<std::shared_ptr<R>> getResourceType()
        {
            const auto& ids = mTypeToIds.at(typeid(std::shared_ptr<R>));

            std::vector<std::shared_ptr<R>> resources;
            resources.reserve(ids.size());

            for (const auto id : ids)
            {
                resources.push_back(std::dynamic_pointer_cast<R>(mIdToResource.at(id)));
            }

            return resources;
        }

    private:
        const VkPhysicalDevice mPhysicalDevice;
        const VkDevice mDevice;
        const VkQueue mGraphicsQueue;
        const CommandPoolPtr& mCommandPool;

        std::vector<RegistryTypes> mRegistries;

        std::hash<std::string> mHasher;

        std::unordered_map<uint64_t, std::any> mIdToEntry;
        std::unordered_map<std::type_index, std::vector<uint64_t>> mTypeToIds;
        std::unordered_map<uint64_t, std::shared_ptr<Handle_I>> mIdToResource;
    };

}
