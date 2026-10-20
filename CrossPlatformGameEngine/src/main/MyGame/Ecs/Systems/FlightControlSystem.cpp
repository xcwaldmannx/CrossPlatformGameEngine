#include "FlightControlSystem.h"

#include "../../../EcsSystem/EcsSystem.h"
#include "../../../WindowManager/InputManager/InputManager.h"
#include "../Components/FlightControlComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/PhysicsBodyComponent.h"

void FlightControlSystem::update(const float delta)
{
    for (const auto e : mEntities)
    {
        auto& [type, aircraft, isControlled] = mSystem->getComponent<FlightControlComponent>(e);
        const auto& trans = mSystem->getComponent<TransformComponent>(e);
        auto& physics = mSystem->getComponent<PhysicsBodyComponent>(e);

        glm::vec3 forward = glm::normalize(trans.mRotation * glm::vec3(0.0f, 0.0f, -1.0f));
        glm::vec3 up      = glm::normalize(trans.mRotation * glm::vec3(0.0f, 1.0f,  0.0f));
        glm::vec3 right   = glm::normalize(trans.mRotation * glm::vec3(1.0f, 0.0f,  0.0f));

        switch (type)
        {
            case AircraftType::HUEY:
            {
                auto& huey = std::get<Huey>(aircraft);

                if (isControlled)
                {
                    if (b3Body_IsValid(physics.mId))
                    {
                        physics.mAppliedForce += calculateCollective(physics.mId, huey.mLift, up, delta);
                        physics.mAppliedTorque += calculateCyclic(physics.mId, forward, right, up, delta);
                        physics.mAppliedTorque += calculateYaw(physics.mId, up, delta);

                        physics.mAppliedForce += calculateDrag(physics.mId, forward, right, up, delta);
                        physics.mAppliedTorque += calculateSCAS(physics.mId, forward, right, up, delta);
                    }
                }

                break;
            }
            case AircraftType::CHINOOK:
                break;
            default:
                break;
        }
    }
}


glm::vec3 FlightControlSystem::calculateCollective(
    const b3BodyId bodyId,
    float& collective,
    const glm::vec3 up,
    const float delta)
{
    const float mass = b3Body_GetMass(bodyId);

    const b3Vec3 b3Velocity = b3Body_GetLinearVelocity(bodyId);

    const glm::vec3 velocity(b3Velocity.x, b3Velocity.y, b3Velocity.z);

    const float gravity = 10.0f;
    const float collectiveStrength = 500.0f;
    const float responsiveness = 50.0f;

    const float minAcceleration = -300.0f;
    const float maxAcceleration = 300.0f;

    const float liftDamping = 3.0f;

    float input = 0.0f;

    if (InputManager::isKeyPressed(GLFW_KEY_W)) input = 1.0f;
    else if (InputManager::isKeyPressed(GLFW_KEY_S)) input = -1.0f;

    const float targetAcceleration = glm::clamp(gravity + input * collectiveStrength, minAcceleration, maxAcceleration);

    const float targetCollective = mass * targetAcceleration;

    const float alpha = 1.0f - expf(-responsiveness * delta);

    collective += (targetCollective - collective) * alpha;

    const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

    return up * collective + damp(velocity, worldUp, liftDamping, mass);
}

glm::vec3 FlightControlSystem::calculateCyclic(
    const b3BodyId bodyId,
    const glm::vec3 forward,
    const glm::vec3 right,
    const glm::vec3 up,
    const float delta)
{
    const float mass = b3Body_GetMass(bodyId);

    const float mousePitchSensitivity = 5.0f;
    const float mouseRollSensitivity = 5.0f;

    const float pitchInput = glm::clamp(static_cast<float>(InputManager::getMouseDeltaY()) * mousePitchSensitivity, -1.0f, 1.0f);

    const float rollInput = glm::clamp(static_cast<float>(-InputManager::getMouseDeltaX()) * mouseRollSensitivity, -1.0f, 1.0f);

    const float maxCyclicTilt = glm::radians(12.0f);

    const float pitchTilt = pitchInput * maxCyclicTilt;
    const float rollTilt = rollInput * maxCyclicTilt;

    const float pitchStrength = 2000.0f;
    const float rollStrength = 2000.0f;

    const glm::vec3 pitchTorque = -right * sinf(pitchTilt) * pitchStrength * mass;

    const glm::vec3 rollTorque = -forward * sinf(rollTilt) * rollStrength * mass;

    return pitchTorque + rollTorque;
}

glm::vec3 FlightControlSystem::calculateYaw(
    const b3BodyId bodyId,
    const glm::vec3 up,
    const float delta)
{
    const float mass = b3Body_GetMass(bodyId);

    float yawInput = 0.0f;

    if (InputManager::isKeyPressed(GLFW_KEY_A)) yawInput = 1.0f;
    else if (InputManager::isKeyPressed(GLFW_KEY_D)) yawInput = -1.0f;

    const float yawStrength = 500.0f;

    return up * yawInput * yawStrength * mass;
}

glm::vec3 FlightControlSystem::calculateSCAS(
    const b3BodyId bodyId,
    const glm::vec3 forward,
    const glm::vec3 right,
    const glm::vec3 up,
    const float delta)
{
    const b3Vec3 angularVel = b3Body_GetAngularVelocity(bodyId);

    const glm::vec3 omega(angularVel.x, angularVel.y, angularVel.z);

    const float mass = b3Body_GetMass(bodyId);

    const float pitchDamping = 250.0f;
    const float yawDamping = 500.0f;
    const float rollDamping = 250.0f;

    return
        damp(omega, right, pitchDamping, mass) +
        damp(omega, up, yawDamping, mass) +
        damp(omega, forward, rollDamping, mass);
}

glm::vec3 FlightControlSystem::calculateDrag(
    const b3BodyId bodyId,
    const glm::vec3 forward,
    const glm::vec3 right,
    const glm::vec3 up,
    const float delta)
{
    const b3Vec3 linearVel = b3Body_GetLinearVelocity(bodyId);

    const glm::vec3 velocity(linearVel.x, linearVel.y, linearVel.z);

    const float mass = b3Body_GetMass(bodyId);

    const float lateralDamping = 8.0f;

    return damp(velocity, right, lateralDamping, mass);
}

glm::vec3 FlightControlSystem::damp(
    const glm::vec3& velocity,
    const glm::vec3& direction,
    const float damping,
    const float mass)
{
    const glm::vec3 axis = glm::normalize(direction);

    return -axis * glm::dot(velocity, axis) * damping * mass;
}
