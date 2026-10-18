#include "FrustumCullingSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/ModelComponent.h"

FrustumCullingSystem::FrustumCullingSystem(ascen::Engine& engine, const std::unordered_map<std::string, Model>& models) :
    mEngine(engine),
    mModels(models) {}

void FrustumCullingSystem::update(const float delta)
{
    mDrawCommands.clear();
    mEntitiesToCull.clear();
    mModelToEntities.clear();
    mDrawMaterialIds.clear();

    mEntitiesToCull.reserve(mEntities.size());

    for (const auto& entityId : mEntities)
    {
        const auto& transform = mSystem->getComponent<TransformComponent>(entityId);
        const auto& model = mSystem->getComponent<ModelComponent>(entityId);

        const auto& m = mModels.at(model.mName);

        ascen::IndexedIndirectDraw drawCommand{};
        drawCommand.vertexOffset = static_cast<int32_t>(m.mVertexOffset);
        drawCommand.firstIndex = m.mIndexOffset;
        drawCommand.indexCount = m.mIndexCount;

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

    uint32_t firstInstance = 0;

    std::unordered_map<uint32_t, const Model*> modelsById;

    for (const auto& [name, model] : mModels)
    {
        modelsById[model.mId] = &model;

    }

    for (const auto& [modelId, entities] : mModelToEntities)
    {
        const auto modelIt = modelsById.find(modelId);

        if (modelIt == modelsById.end())
        {
            throw std::runtime_error("FrustumCullingSystem: Model ID not found");
        }

        const Model& model = *modelIt->second;

        const uint32_t instanceCount = static_cast<uint32_t>(entities.size());

        for (const auto& [entityId, entity] : entities)
        {
            mEntitiesToCull.push_back(entity);
        }

        for (const auto& mesh : model.mMeshes)
        {
            ascen::IndexedIndirectDraw drawCommand{};

            drawCommand.firstIndex = model.mIndexOffset + mesh.mIndexOffset;
            drawCommand.indexCount = mesh.mIndexCount;
            drawCommand.vertexOffset = static_cast<int32_t>(model.mVertexOffset);

            drawCommand.firstInstance = firstInstance;
            drawCommand.instanceCount = instanceCount;

            mDrawCommands.push_back(drawCommand);

            mDrawMaterialIds.push_back(static_cast<int32_t>(model.mMaterialOffset + mesh.mMaterialIndex));
        }

        firstInstance += instanceCount;
    }

    if (!mEntitiesToCull.empty())
    {
        mEngine.uploadBuffer("BUFFER_ENTITY", &mEntitiesToCull[0], mEntitiesToCull.size(), sizeof(Entity), 0);
    }

    if (!mDrawCommands.empty())
    {
        mEngine.uploadBuffer("BUFFER_DRAWS", &mDrawCommands[0], mDrawCommands.size(), sizeof(ascen::IndexedIndirectDraw), 0);

        mEngine.uploadBuffer("BUFFER_DRAW_MATERIAL", &mDrawMaterialIds[0], mDrawMaterialIds.size(), sizeof(int32_t), 0);
    }
}
