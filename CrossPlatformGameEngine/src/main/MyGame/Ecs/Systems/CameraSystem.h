#pragma once

#include "../../../EcsSystem/SystemManager/System/System_I.h"
#include "../../../Engine/Core/Engine.h"

class CameraSystem : public System_I
{
public:
    CameraSystem(ascen::Engine& engine);

    void update(const float delta) override;

private:
    ascen::Engine& mEngine;

    EntityId mActiveCamera = ENTITY_MAX;
};