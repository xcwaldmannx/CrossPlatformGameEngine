#include "FrustumCullingSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/ModelComponent.h"

FrustumCullingSystem::FrustumCullingSystem(ascen::Engine& engine, const std::unordered_map<std::string, MyModel>& models) :
    mEngine(engine),
    mModels(models){}

void FrustumCullingSystem::update(float deltaTime)
{
    mModelToDrawCommands.clear();
    mDrawCommands.clear();

    mModelToEntities.clear();

    mEntitiesToCull.clear();
    mEntitiesToCull.reserve(mEntities.size());

    for (const auto& entityId : mEntities)
    {
        const auto& transform = mComponentManager->getComponent<TransformComponent>(entityId);
        const auto& model = mComponentManager->getComponent<ModelComponent>(entityId);

        const auto& m = mModels.at(model.mName);

        Entity e{};
        e.mPosition  = transform.mPosition;
        e.mRotation  = transform.mRotation;
        e.mScale     = transform.mScale;
        e.mIsVisible = 1;

        if (!mModelToDrawCommands.contains(m.mModelId))
        {
            ascen::IndirectBuffer::IndexedIndirectCommand drawCommand{};
            drawCommand.vertexOffset = static_cast<int32_t>(m.mVertexOffset);
            drawCommand.firstIndex = m.mIndexOffset;
            drawCommand.indexCount = m.mIndexCount;

            mModelToDrawCommands[m.mModelId] = drawCommand;
        }

        mModelToEntities[m.mModelId].push_back(e);
    }

    uint32_t firstInstance = 0;

    for (const auto& [model, entities] : mModelToEntities)
    {
        mModelToDrawCommands[model].firstInstance = firstInstance;
        mModelToDrawCommands[model].instanceCount = entities.size();

        mDrawCommands.push_back(mModelToDrawCommands[model]);
        mEntitiesToCull.append_range(entities);

        firstInstance += entities.size();
    }

    if (!mEntitiesToCull.empty() && !mDrawCommands.empty())
    {
        mEngine.resource().uploadBuffer("BUFFER_ENTITY",
            &mEntitiesToCull[0], mEntitiesToCull.size(), sizeof(Entity));

        mEngine.resource().uploadBuffer("BUFFER_DRAWS",
            &mDrawCommands[0], mDrawCommands.size(), sizeof(ascen::IndirectBuffer::IndexedIndirectCommand));
    }
}
