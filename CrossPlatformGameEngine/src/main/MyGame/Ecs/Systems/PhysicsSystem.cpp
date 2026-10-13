#include "PhysicsSystem.h"

#include "../../../EcsSystem/EcsSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/ModelComponent.h"
#include "../Components/PhysicsBodyComponent.h"

#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>

PhysicsSystem::PhysicsSystem(ModelHandler& modelHandler, b3WorldId& worldId) :
    mWorldId(worldId), mModelHandler(modelHandler)
{
    b3BodyDef groundBodyDef = b3DefaultBodyDef();
    groundBodyDef.position = (b3Vec3){ 0.0f, 0.0f, 0.0f };

    b3BodyId groundId = b3CreateBody(mWorldId, &groundBodyDef);

    b3BoxHull groundBox = b3MakeBoxHull(1024.0f, 1.0f, 1024.0f);

    b3ShapeDef groundShapeDef = b3DefaultShapeDef();
    b3CreateHullShape(groundId, &groundShapeDef, &groundBox.base);
}

PhysicsSystem::~PhysicsSystem()
{
    b3DestroyWorld(mWorldId);
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
            const auto& rot = transform.mRotation;
            const auto& scale = transform.mScale;

            b3BodyDef bodyDef = b3DefaultBodyDef();
            bodyDef.type = b3_dynamicBody;
            bodyDef.position = b3Vec3{ pos.x, pos.y, pos.z };
            bodyDef.rotation = b3MakeQuatFromAxisAngle({ rot.x, rot.y, rot.z }, rot.w);
            b3BodyId bodyId = b3CreateBody(mWorldId, &bodyDef);
            mBodies.emplace(e, bodyId);

            const auto& model = mSystem->getComponent<ModelComponent>(e);
            const auto& m = mModelHandler.getModel(model.mName);

            glm::vec3 center = (m.mBoundsMax+ m.mBoundsMin) * 0.5f;
            glm::vec3 halfExtents = glm::abs((m.mBoundsMax - m.mBoundsMin) * 0.5f);

            center *= scale;
            halfExtents *= scale;

            b3BoxHull box = b3MakeOffsetBoxHull(halfExtents.x, halfExtents.y, halfExtents.z, { center.x, center.y, center.z });

            b3ShapeDef shapeDef = b3DefaultShapeDef();
            shapeDef.density = 3.0f;
            shapeDef.baseMaterial.friction = 0.5f;

            b3CreateHullShape(bodyId, &shapeDef, &box.base);
        }

        const auto bodyTrans = b3Body_GetTransform(mBodies.at(e));

        auto& bodyId = mBodies.at(e);
        b3Body_ApplyLinearImpulseToCenter(bodyId, b3Vec3(physicsBody.mAppliedForce.x, physicsBody.mAppliedForce.y, physicsBody.mAppliedForce.z), true);
        physicsBody.mAppliedForce = glm::vec3(0);

        const auto bodyPos = bodyTrans.p;
        transform.mPosition = { bodyPos.x, bodyPos.y, bodyPos.z };

        const auto bodyRot = bodyTrans.q;
        const glm::quat q(bodyRot.s, bodyRot.v.x, bodyRot.v.y, bodyRot.v.z);

        transform.mRotation = glm::eulerAngles(q);
    }

    b3World_Step(mWorldId, delta, 8);
}
