#include "PhysicsSystem.h"

#include "../../../EcsSystem/EcsSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/PhysicsBodyComponent.h"

PhysicsSystem::PhysicsSystem()
{
    b3WorldDef worldDef = b3DefaultWorldDef();
    worldDef.gravity = { 0.0f, -9.81f, 0.0f };
    mWorldId  = b3CreateWorld(&worldDef);

    b3BodyDef groundBodyDef = b3DefaultBodyDef();
    groundBodyDef.position = (b3Vec3){ 0.0f, -10.0f, 0.0f };

    b3BodyId groundId = b3CreateBody(mWorldId, &groundBodyDef);

    b3BoxHull groundBox = b3MakeBoxHull(1024.0f, 1.0f, 1024.0f);

    b3ShapeDef groundShapeDef = b3DefaultShapeDef();
    b3CreateHullShape(groundId, &groundShapeDef, &groundBox.base);
}

void PhysicsSystem::update(const float delta)
{
    for (const auto& e : mEntities)
    {
        auto& transform = mSystem->getComponent<TransformComponent>(e);
        auto& physicsBody = mSystem->getComponent<PhysicsBodyComponent>(e);

        if (!mBodies.contains(e))
        {
            const auto& pos = transform.mPosition;

            b3BodyDef bodyDef = b3DefaultBodyDef();
            bodyDef.type = b3_dynamicBody;
            bodyDef.position = b3Vec3{ pos.x, pos.y, pos.z };
            b3BodyId bodyId = b3CreateBody(mWorldId, &bodyDef);
            mBodies.emplace(e, bodyId);

            b3BoxHull dynamicBox = b3MakeCubeHull(2.0f);

            b3ShapeDef shapeDef = b3DefaultShapeDef();
            shapeDef.density = 3.0f;
            shapeDef.baseMaterial.friction = 0.3f;

            b3CreateHullShape(bodyId, &shapeDef, &dynamicBox.base);
        }

        const auto bodyPos = b3Body_GetPosition(mBodies.at(e));
        transform.mPosition = { bodyPos.x, bodyPos.y, bodyPos.z };

        const auto bodyRot = b3Body_GetRotation(mBodies.at(e));
        transform.mRotation = { bodyRot.v.x, bodyRot.v.y, bodyRot.v.z };
    }

    b3World_Step(mWorldId, 1.0f / 60.0f, 4);
}
