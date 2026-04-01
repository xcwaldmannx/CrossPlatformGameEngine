#include "SimpleRenderSystem.h"

#include "FrustumCullingSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/ModelComponent.h"

SimpleRenderSystem::SimpleRenderSystem(
    ascen::Engine& engine,
    const std::unordered_map<std::string, MyModel>& models) :
    mEngine(engine),
    mModels(models) {}

void SimpleRenderSystem::update(float deltaTime)
{
    for (const auto& entity : mDirtyEntities)
    {
        const auto& model = mEngine.ecs().getComponent<ModelComponent>(entity);


    }
}