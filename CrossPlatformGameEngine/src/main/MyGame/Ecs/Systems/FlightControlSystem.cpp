
#include "FlightControlSystem.h"

#include "../../../EcsSystem/EcsSystem.h"
#include "../../../WindowManager/InputManager/InputManager.h"
#include "../Components/FlightControlComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/PhysicsComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cmath>

namespace
{
    constexpr float GRAVITY = 9.81f;

    constexpr float CLIMB_ACCEL = 25.0f;
    constexpr float VERTICAL_RESPONSE = 25.0f;

    constexpr float MOUSE_SENSITIVITY = 0.018f;
    constexpr float MAX_PITCH_RATE = 2.8f;
    constexpr float MAX_ROLL_RATE = 3.2f;
    constexpr float MAX_YAW_RATE = 1.8f;
    constexpr float ANGULAR_RESPONSE = 2.0f;
    constexpr float MOUSE_RATE_DECAY = 2.0f;

    constexpr float FORWARD_ASSIST = 25.0f;
    constexpr float FORWARD_DRAG = 0.035f;
    constexpr float LATERAL_DRAG = 0.22f;

    glm::vec3 toGlm(const b3Vec3& v)
    {
        return {v.x, v.y, v.z};
    }

    b3Vec3 toB3(const glm::vec3& v)
    {
        return {v.x, v.y, v.z};
    }
}

void FlightControlSystem::update(const float delta)
{
    const float dt = glm::clamp(delta, 0.0f, 0.05f);
    if (dt <= 0.0f) return;

    for (const auto e : mEntities)
    {
        auto& [type, aircraft, isControlled] = mSystem->getComponent<FlightControlComponent>(e);
        auto& physics = mSystem->getComponent<PhysicsComponent>(e);
        const auto& trans = mSystem->getComponent<TransformComponent>(e);

        if (!isControlled || !b3Body_IsValid(physics.mId)) continue;

        switch (type)
        {
            case AircraftType::HUEY:
            {
                auto& huey = std::get<Huey>(aircraft);

                const b3BodyId bodyId = physics.mId;
                const glm::quat orientation = trans.mRotation;

                const glm::vec3 forward = glm::normalize(orientation * glm::vec3(0.0f, 0.0f, -1.0f));
                const glm::vec3 right = glm::normalize(orientation * glm::vec3(1.0f, 0.0f, 0.0f));
                const glm::vec3 up = glm::normalize(orientation * glm::vec3(0.0f, 1.0f, 0.0f));

                const float mass = b3Body_GetMass(bodyId);
                const glm::vec3 velocity = toGlm(b3Body_GetLinearVelocity(bodyId));
                const glm::vec3 omega = toGlm(b3Body_GetAngularVelocity(bodyId));

                // Collective: W climbs, S reduces lift, neutral hovers when level.
                float collectiveInput = 0.0f;

                if (InputManager::isKeyPressed(GLFW_KEY_W)) collectiveInput = 1.0f;
                else if (InputManager::isKeyPressed(GLFW_KEY_S)) collectiveInput = -1.0f;

                const float targetLift = collectiveInput > 0.0f ? GRAVITY + CLIMB_ACCEL : collectiveInput < 0.0f ? 0.0f : GRAVITY;
                const float verticalAlpha = 1.0f - std::exp(-VERTICAL_RESPONSE * dt);

                huey.mLift += (targetLift - huey.mLift) * verticalAlpha;
                huey.mLift = glm::max(huey.mLift, 0.0f);

                // Rotor thrust is always nonnegative.
                // Box3D gravity causes descent when thrust is below weight.
                glm::vec3 force = up * huey.mLift * mass;

                // Horizontal aerodynamic drag.
                force += damp(velocity, forward, FORWARD_DRAG, mass);
                force += damp(velocity, right, LATERAL_DRAG, mass);

                // Arcade acceleration in the direction of rotor tilt.
                const glm::vec3 horizontalTilt(up.x, 0.0f, up.z);
                force += horizontalTilt * FORWARD_ASSIST * mass;

                b3Body_ApplyForceToCenter(bodyId, toB3(force), true);

                // Cyclic: mouse controls pitch and roll.
                const float mouseX = static_cast<float>(InputManager::getMouseDeltaX());
                const float mouseY = static_cast<float>(InputManager::getMouseDeltaY());

                huey.mPitchRate = glm::clamp(huey.mPitchRate - mouseY * MOUSE_SENSITIVITY, -MAX_PITCH_RATE, MAX_PITCH_RATE);
                huey.mRollRate = glm::clamp(huey.mRollRate - mouseX * MOUSE_SENSITIVITY, -MAX_ROLL_RATE, MAX_ROLL_RATE);

                // Pedals: A/D controls yaw.
                float yawInput = 0.0f;

                if (InputManager::isKeyPressed(GLFW_KEY_A)) yawInput = 1.0f;
                else if (InputManager::isKeyPressed(GLFW_KEY_D)) yawInput = -1.0f;

                const glm::vec3 targetOmega = right * huey.mPitchRate - forward * huey.mRollRate + up * yawInput * MAX_YAW_RATE;
                const float angularAlpha = 1.0f - std::exp(-ANGULAR_RESPONSE * dt);
                const glm::vec3 newOmega = glm::mix(omega, targetOmega, angularAlpha);

                b3Body_SetAngularVelocity(bodyId, toB3(newOmega));

                // Smoothly decay cyclic input.
                const float decay = std::exp(-MOUSE_RATE_DECAY * dt);
                huey.mPitchRate *= decay;
                huey.mRollRate *= decay;

                break;
            }

            case AircraftType::CHINOOK:
                break;

            default:
                break;
        }
    }
}

glm::vec3 FlightControlSystem::damp(const glm::vec3& velocity, const glm::vec3& direction, const float damping, const float mass)
{
    const glm::vec3 axis = glm::normalize(direction);
    return -axis * glm::dot(velocity, axis) * damping * mass;
}
