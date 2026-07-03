#include "RegistryManager.h"

#include <any>

using namespace ascen;

RegistryManager::RegistryManager(const DescriptorFactory& descriptorFactory)
{
    mRegistries.push_back(VertexRegistry{});
    mRegistries.push_back(DescriptorPoolRegistry{ descriptorFactory });
    mRegistries.push_back(DescriptorSetLayoutRegistry{ descriptorFactory });
    mRegistries.push_back(DescriptorSetRegistry{ descriptorFactory, &mIdToResource });
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

            std::type_index typeId = std::type_index(typeid(CurrentEntryType));

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
