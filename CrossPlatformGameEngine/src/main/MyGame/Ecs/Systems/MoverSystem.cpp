#include "MoverSystem.h"

#include "../../../EcsSystem/EcsSystem.h"
#include "../Components/MoverComponent.h"
#include "../Components/TransformComponent.h"

#include <GLFW/glfw3.h>
#include <box3d/box3d.h>

MoverSystem::MoverSystem(b3WorldId& worldId) : mWorldId(worldId) {}

void MoverSystem::update(float delta)
{
    b3Capsule capsuleMover;
    capsuleMover.center1 = (b3Vec3){ 0.0f, 0.5f, 0.0f };  // bottom sphere center
    capsuleMover.center2 = (b3Vec3){ 0.0f, 2.0f, 0.0f };  // top sphere center
    capsuleMover.radius  = 0.5f;

    for (const auto& e : mEntities)
    {
        const auto& mover = mSystem->getComponent<MoverComponent>(e);
        auto& transform = mSystem->getComponent<TransformComponent>(e);

        const auto& pos = transform.mPosition;
        const auto& rot = transform.mRotation;

        double mouseDeltaX = InputManager::getMouseDeltaX();
        double mouseDeltaY = InputManager::getMouseDeltaY();

        float sensitivity = 0.0025f;
        mYaw   -= static_cast<float>(mouseDeltaX) * mover.mLookSpeed * sensitivity;
        mPitch -= static_cast<float>(mouseDeltaY) * mover.mLookSpeed * sensitivity;

        mPitch = glm::clamp(mPitch, glm::radians(-89.0f), glm::radians(89.0f));

        const glm::quat yawRotation = glm::angleAxis(mYaw, glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::quat pitchRotation = glm::angleAxis(mPitch, glm::vec3(1.0f, 0.0f, 0.0f));

        transform.mRotation = glm::normalize(yawRotation * pitchRotation);

        const glm::vec3 forward = rot * glm::vec3(0.0f, 0.0f, -1.0f);
        const glm::vec3 right = rot * glm::vec3(1.0f, 0.0f, 0.0f);
        const glm::vec3 up = rot * glm::vec3(0.0f, 1.0f, 0.0f);

        glm::vec3 direction { 0, 0, 0 };

        if (InputManager::isKeyPressed(GLFW_KEY_A))
        {
            direction -= right;
        }

        if (InputManager::isKeyPressed(GLFW_KEY_D))
        {
            direction += right;
        }

        if (InputManager::isKeyPressed(GLFW_KEY_W))
        {
            direction += forward;
        }

        if (InputManager::isKeyPressed(GLFW_KEY_S))
        {
            direction -= forward;
        }

        if (InputManager::isKeyJustPressed(GLFW_KEY_SPACE))
        {
            direction -= up;
        }

        if (glm::dot(direction, direction) > 0.0f)
        {
            direction = glm::normalize(direction);
        }

        direction *= mover.mWalkSpeed;

        MoverContext context{};

        const b3Vec3 origin = { pos.x, pos.y, pos.z };
        const b3Vec3 translation = { direction.x * delta, direction.y * delta, direction.z * delta };
        const b3QueryFilter filter = b3DefaultQueryFilter();
        const float fraction = b3World_CastMover(mWorldId, origin, &capsuleMover, translation, filter, castMoverCallback, &context);

        const b3Vec3 safeDelta = b3MulSV(fraction, translation);

        transform.mPosition.x += safeDelta.x;
        transform.mPosition.y += safeDelta.y;
        transform.mPosition.z += safeDelta.z;

        capsuleMover.center1 += safeDelta;
        capsuleMover.center2 += safeDelta;

        if (fraction < 1.0f && context.mHasHit && b3Body_IsValid(context.mHitBodyId))
        {
            b3Vec3 pushDirection = b3Normalize(context.mPosition - b3Vec3(pos.x, pos.y, pos.z));
            b3Vec3 pushStrength = b3MulSV(10.0f, pushDirection);

            b3Body_ApplyLinearImpulseToCenter(context.mHitBodyId, pushStrength, true);
        }
    }
}

bool MoverSystem::castMoverCallback(const b3ShapeId shapeId, void* context)
{
    auto* ctx = static_cast<MoverContext*>(context);

    // Always ignore your own player geometry
    const bool same = B3_ID_EQUALS(ctx->mSelfShapeId, shapeId);
    // if (!same) return false;

    const b3BodyId bodyId = b3Shape_GetBody(shapeId);
    const b3BodyType bodyType = b3Body_GetType(bodyId);

    // If it's a dynamic, physics-enabled object, log it
    if (bodyType == b3_dynamicBody)
    {
        ctx->mHitBodyId = bodyId;
        ctx->mHasHit = true;
        ctx->mPosition = b3Body_GetPosition(bodyId);
    }

    return true;
}

bool MoverSystem::collideMoverCallback(b3ShapeId shapeId, const b3PlaneResult* plane, int planeCount, void* context)
{
    auto* ctx = static_cast<MoverContext*>(context);

    // Always ignore your own player geometry
    const bool same = B3_ID_EQUALS(ctx->mSelfShapeId, shapeId);
    // if (!same) return false;

    const b3BodyId bodyId = b3Shape_GetBody(shapeId);
    const b3BodyType bodyType = b3Body_GetType(bodyId);

    if (bodyType == b3_dynamicBody)
    {
    }

    return true;
}
