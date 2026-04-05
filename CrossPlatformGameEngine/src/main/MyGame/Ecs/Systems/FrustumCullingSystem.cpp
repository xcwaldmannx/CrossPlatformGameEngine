#include "FrustumCullingSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/ModelComponent.h"

FrustumCullingSystem::FrustumCullingSystem(ascen::Engine& engine, const std::unordered_map<std::string, MyModel>& models) :
    mEngine(engine),
    mModels(models){}

void FrustumCullingSystem::update(const float delta)
{
    mDrawCommands.clear();
    mDrawCommands.reserve(mEntities.size());

    mEntitiesToCull.clear();
    mEntitiesToCull.reserve(mEntities.size());

    for (const auto& entityId : mDirtyEntities)
    {
        const auto& transform = mSystem->getComponent<TransformComponent>(entityId);
        const auto& model = mSystem->getComponent<ModelComponent>(entityId);

        const auto& m = mModels.at(model.mName);

        ascen::IndirectBuffer::IndexedIndirectCommand drawCommand{};
        drawCommand.vertexOffset = static_cast<int32_t>(m.mVertexOffset);
        drawCommand.firstIndex = m.mIndexOffset;
        drawCommand.indexCount = m.mIndexCount;

        if (!mModelToDrawCommands.contains(m.mModelId))
        {
            mModelToDrawCommands[m.mModelId] = drawCommand;

            Entity e{};
            e.mPosition  = transform.mPosition;
            e.mRotation  = transform.mRotation;
            e.mScale     = transform.mScale;
            e.mBoundsPos = m.mBoundsPos;
            e.mBoundsNeg = m.mBoundsNeg;
            mModelToEntities[m.mModelId][entityId] = e;
        }
        else
        {
            Entity& e = mModelToEntities[m.mModelId][entityId];
            e.mPosition  = transform.mPosition;
            e.mRotation  = transform.mRotation;
            e.mScale     = transform.mScale;
            e.mBoundsPos = m.mBoundsPos;
            e.mBoundsNeg = m.mBoundsNeg;
        }

    }

    uint32_t firstInstance = 0;

    for (const auto& [model, entities] : mModelToEntities)
    {
        mModelToDrawCommands[model].firstInstance = firstInstance;
        mModelToDrawCommands[model].instanceCount = entities.size();

        mDrawCommands.push_back(mModelToDrawCommands[model]);

        for (auto const& [key, value] : entities) {
            mEntitiesToCull.push_back(value);
        }

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
