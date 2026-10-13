#pragma once

#include "../../../EcsSystem/SystemManager/System/System_I.h"

#include <unordered_map>

#include <box3d/box3d.h>

#include "../../ModelHandler/ModelHandler.h"

class PhysicsSystem : public System_I
{
public:
    PhysicsSystem(ModelHandler& modelHandler, b3WorldId& worldId);
    ~PhysicsSystem();

    void update(float delta) override;

private:
    ModelHandler& mModelHandler;
    b3WorldId& mWorldId;
    std::unordered_map<EntityId, b3BodyId> mBodies;
};
