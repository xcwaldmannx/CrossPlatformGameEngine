#include "FrustumCullingSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/ModelComponent.h"

FrustumCullingSystem::FrustumCullingSystem(ascen::Engine& engine, const std::unordered_map<std::string, Model>& models) :
    mEngine(engine),
    mModels(models){}

void FrustumCullingSystem::update(const float delta)
{
    mDrawCommands.clear();
    mDrawCommands.reserve(mEntities.size());

    mEntitiesToCull.clear();
    mEntitiesToCull.reserve(mEntities.size());

    mModelToEntities.clear();
    mModelToDrawCommands.clear();

    for (const auto& entityId : mEntities)
    {
        const auto& transform = mSystem->getComponent<TransformComponent>(entityId);
        const auto& model = mSystem->getComponent<ModelComponent>(entityId);

        const auto& m = mModels.at(model.mName);

        ascen::IndexedIndirectDraw drawCommand{};
        drawCommand.vertexOffset = static_cast<int32_t>(m.mVertexOffset);
        drawCommand.firstIndex = m.mIndexOffset;
        drawCommand.indexCount = m.mIndexCount;

        if (!mModelToDrawCommands.contains(m.mId))
        {
            mModelToDrawCommands[m.mId] = drawCommand;

            Entity e{};
            e.mPosition   = transform.mPosition;
            e.mEntityId   = entityId;
            e.mRotation   = transform.mRotation;
            e.mIsSelected = model.mIsSelected;
            e.mScale      = transform.mScale;
            e.mBoundsPos  = m.mBoundsMax;
            e.mBoundsNeg  = m.mBoundsMin;
            mModelToEntities[m.mId][entityId] = e;
        }
        else
        {
            Entity& e = mModelToEntities[m.mId][entityId];
            e.mPosition   = transform.mPosition;
            e.mEntityId   = entityId;
            e.mRotation   = transform.mRotation;
            e.mIsSelected = model.mIsSelected;
            e.mScale      = transform.mScale;
            e.mBoundsPos  = m.mBoundsMax;
            e.mBoundsNeg  = m.mBoundsMin;
        }

    }

    uint32_t firstInstance = 0;

    for (const auto& [model, entities] : mModelToEntities)
    {
        mModelToDrawCommands[model].firstInstance = firstInstance;
        mModelToDrawCommands[model].instanceCount = entities.size();

        mDrawCommands.push_back(mModelToDrawCommands[model]);

        for (auto const& [key, value] : entities)
        {
            mEntitiesToCull.push_back(value);
        }

        firstInstance += entities.size();
    }

    if (!mEntitiesToCull.empty() && !mDrawCommands.empty())
    {
        mEngine.uploadBuffer("BUFFER_ENTITY",
            &mEntitiesToCull[0], mEntitiesToCull.size(), sizeof(Entity), 0);

        mEngine.uploadBuffer("BUFFER_DRAWS",
            &mDrawCommands[0], mDrawCommands.size(), sizeof(ascen::IndexedIndirectDraw), 0);
    }
}
