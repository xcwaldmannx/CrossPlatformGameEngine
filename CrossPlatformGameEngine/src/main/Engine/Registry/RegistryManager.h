#pragma once

#include "../Registry2/Registry_I.h"
#include "../Registry2/VertexRegistry.h"
#include "../Registry2/DescriptorPoolRegistry.h"
#include "../Registry2/DescriptorSetLayoutRegistry.h"
#include "../Registry2/DescriptorSetRegistry.h"

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
        DescriptorSetRegistry>;

    template <typename T>
    struct RegistryTraits;

    template <typename EntryT, typename ResourceT>
    struct RegistryTraits<Registry_I<EntryT, ResourceT>> {
        using EntryType = EntryT;
        using ResourceType = ResourceT;
    };

    template<typename T, typename U>
    concept Derived = std::is_base_of_v<U, T>;

    class RegistryManager
    {
    public:
        RegistryManager(const DescriptorFactory& descriptorFactory);

        template<Derived<registry::Entry> E>
        void registerResource(const E& entry)
        {
            const uint64_t id = mHasher(entry.mName);
            mIdToEntry.emplace(id, std::make_shared<E>(entry));

            const std::type_index typeId = std::type_index(typeid(E));
            mTypeToIds[typeId].push_back(id);
        }

        void reconstruct();

    private:
        std::vector<RegistryTypes> mRegistries;

        VertexRegistry mVertexRegistry;

        std::hash<std::string> mHasher;

        std::unordered_map<uint64_t, std::shared_ptr<registry::Entry>> mIdToEntry;
        std::unordered_map<std::type_index, std::vector<uint64_t>> mTypeToIds;
        std::unordered_map<uint64_t, registry::Resource> mIdToResource;
    };

}
