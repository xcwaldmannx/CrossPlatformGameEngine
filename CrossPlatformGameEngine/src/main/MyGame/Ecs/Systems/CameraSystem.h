#pragma once

#include "../../../EcsSystem/SystemManager/System/System_I.h"
#include "../../../Engine/Core/Engine.h"

struct CameraComponent;

class CameraSystem : public System_I
{
public:
    CameraSystem(ascen::Engine& engine);

    void update(const float delta) override;

    bool getActiveCamera(CameraComponent& camera) const;

private:
    ascen::Engine& mEngine;

    EntityId mActiveCamera = ENTITY_MAX;
};