#pragma once

#include "../../../EcsSystem/SystemManager/System/System_I.h"

#include <unordered_map>

#include <box3d/box3d.h>

class PhysicsSystem : public System_I
{
public:
    PhysicsSystem();

    void update(float delta) override;

private:
    b3WorldId mWorldId;
    std::unordered_map<EntityId, b3BodyId> mBodies;
};