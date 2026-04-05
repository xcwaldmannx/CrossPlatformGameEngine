#include "PhysicsSystem.h"

#include "../../../EcsSystem/EcsSystem.h"
#include "../Components/TransformComponent.h"

PhysicsSystem::PhysicsSystem()
{

}

void PhysicsSystem::update(const float delta)
{
    for (const auto& e : mEntities)
    {
        auto& transform = mSystem->getComponent<TransformComponent>(e);

        if (transform.mPosition.y > 0)
        {
            transform.mPosition.y -= 2.0f * delta;
        }
        else
        {
            transform.mPosition.y = 0;
        }
    }
}
