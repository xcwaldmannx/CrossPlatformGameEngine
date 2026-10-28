#include "PhysicsSystem.h"

#include "../../../EcsSystem/EcsSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/ModelComponent.h"
#include "../Components/PhysicsComponent.h"

#include <glm/gtc/quaternion.hpp>

PhysicsSystem::PhysicsSystem(b3WorldId& worldId) : mWorldId(worldId)
{
    // b3BodyDef groundBodyDef = b3DefaultBodyDef();
    // groundBodyDef.position = (b3Vec3){ 0.0f, 0.0f, 0.0f };
    //
    // b3BodyId groundId = b3CreateBody(mWorldId, &groundBodyDef);
    //
    // b3BoxHull groundBox = b3MakeBoxHull(8192.0f, 1.0f, 8192.0f);
    //
    // b3ShapeDef groundShapeDef = b3DefaultShapeDef();
    // b3CreateHullShape(groundId, &groundShapeDef, &groundBox.base);
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
        auto& physicsBody = mSystem->getComponent<PhysicsComponent>(e);

        if (!mBodies.contains(e))
        {
            b3BodyDef bodyDef = b3DefaultBodyDef();

            switch (physicsBody.mBodyType)
            {
                case PhysicsBodyType::STATIC:
                    bodyDef.type = b3_staticBody;
                    break;
                case PhysicsBodyType::DYNAMIC:
                    bodyDef.type = b3_dynamicBody;
                    break;
                default:
                    break;
            }

            const auto& pos = transform.mPosition;
            const auto& rot = transform.mRotation;
            const auto& scale = transform.mScale;

            bodyDef.position = b3Vec3{ pos.x, pos.y, pos.z };
            auto normRot = glm::normalize(rot);
            bodyDef.rotation = b3Quat{ normRot.x, normRot.y, normRot.z, normRot.w };
            b3BodyId bodyId = b3CreateBody(mWorldId, &bodyDef);
            physicsBody.mId = bodyId;
            mBodies.emplace(e, bodyId);

            if (auto* collider = std::get_if<Box>(&physicsBody.mCollider))
            {
                glm::vec3 center = { 0, 0, 0 };
                glm::vec3 halfExtents = (collider->mLengths) * 0.5f;

                center *= scale;
                halfExtents *= scale;

                b3BoxHull box = b3MakeOffsetBoxHull(halfExtents.x, halfExtents.y, halfExtents.z, { center.x, center.y, center.z });

                b3ShapeDef shapeDef = b3DefaultShapeDef();
                shapeDef.density = physicsBody.mDensity;
                shapeDef.baseMaterial.friction = 0.5f;

                b3CreateHullShape(bodyId, &shapeDef, &box.base);
            }

            if (auto* collider = std::get_if<Sphere>(&physicsBody.mCollider))
            {
                b3Sphere sphere = { { 0, 0, 0 }, collider->mRadius };

                b3ShapeDef shapeDef = b3DefaultShapeDef();
                shapeDef.density = physicsBody.mDensity;
                shapeDef.baseMaterial.friction = 0.5f;

                b3CreateSphereShape(bodyId, &shapeDef, &sphere);
            }

            if (auto* collider = std::get_if<Mesh>(&physicsBody.mCollider))
            {
                std::vector<b3Vec3> vertices;
                vertices.reserve(collider->mVertices.size());

                for (const glm::vec3& vertex : collider->mVertices)
                {
                    vertices.push_back({ vertex.x, vertex.y, vertex.z });
                }

                b3MeshDef meshDef{};
                meshDef.vertices = vertices.data();
                meshDef.vertexCount = static_cast<int>(vertices.size());
                meshDef.indices = collider->mIndices.data();
                meshDef.triangleCount = static_cast<int>(collider->mIndices.size() / 3);
                meshDef.weldVertices = true;
                meshDef.identifyEdges = true;

                b3MeshData* mesh = b3CreateMesh(&meshDef, nullptr, 0);

                if (mesh == nullptr) throw std::runtime_error("Failed to create terrain collision mesh");

                b3ShapeDef shapeDef = b3DefaultShapeDef();
                shapeDef.baseMaterial.friction = 0.5f;

                b3Vec3 meshScale = { scale.x, scale.y, scale.z };
                b3CreateMeshShape(bodyId, &shapeDef, mesh, meshScale);
            }
        }


        switch (physicsBody.mBodyType)
        {
            case PhysicsBodyType::STATIC:
                break;
            case PhysicsBodyType::DYNAMIC:
            {
                const auto bodyTrans = b3Body_GetTransform(mBodies.at(e));

                auto& bodyId = mBodies.at(e);

                // linear
                b3Body_ApplyForceToCenter(bodyId, b3Vec3(physicsBody.mAppliedForce.x, physicsBody.mAppliedForce.y, physicsBody.mAppliedForce.z), true);
                physicsBody.mAppliedForce = glm::vec3(0);

                // rotational
                b3Body_ApplyTorque(bodyId, b3Vec3(physicsBody.mAppliedTorque.x, physicsBody.mAppliedTorque.y, physicsBody.mAppliedTorque.z), true);
                physicsBody.mAppliedTorque = glm::vec3(0);

                const auto bodyPos = bodyTrans.p;
                transform.mPosition = { bodyPos.x, bodyPos.y, bodyPos.z };

                const auto bodyRot = bodyTrans.q;
                const glm::quat q(bodyRot.s, bodyRot.v.x, bodyRot.v.y, bodyRot.v.z);

                transform.mRotation = glm::eulerAngles(q);
                break;
            }
            default:
                break;
        }
    }

    b3World_Step(mWorldId, delta, 4);
}
