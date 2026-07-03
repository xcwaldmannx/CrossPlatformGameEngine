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

    private:
        std::vector<RegistryTypes> mRegistries;

        std::hash<std::string> mHasher;

        std::unordered_map<uint64_t, std::shared_ptr<registry::Entry>> mIdToEntry;
        std::unordered_map<std::type_index, std::vector<uint64_t>> mTypeToIds;
        std::unordered_map<uint64_t, registry::Resource> mIdToResource;
    };

}
