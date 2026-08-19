#pragma once

#include <unordered_map>
#include <box3d/types.h>

#include "../../../EcsSystem/SystemManager/System/System_I.h"
#include "../../../WindowManager/WindowManager.h"

struct MoverContext
{
    b3ShapeId mSelfShapeId = b3_nullShapeId;
    b3BodyId mHitBodyId = b3_nullBodyId;
    bool mHasHit = false;
    b3Vec3 mPosition = b3Vec3(0.0f);
};

class MoverSystem : public System_I
{
public:
    MoverSystem(b3WorldId& worldId);

    void update(float delta) override;

private:
    static bool castMoverCallback(b3ShapeId shapeId, void* context);
    static bool collideMoverCallback(b3ShapeId shapeId, const b3PlaneResult* plane, int planeCount, void* context);

private:
    b3WorldId& mWorldId;

    std::unordered_map<EntityId, b3Capsule> mMovers;

    float mYaw = 0.0f;
    float mPitch = 0.0f;
};
