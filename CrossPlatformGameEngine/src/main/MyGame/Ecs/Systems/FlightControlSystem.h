#pragma once

#include "../../../EcsSystem/SystemManager/System/System_I.h"

#include <glm/glm.hpp>
#include <box3d/box3d.h>

class FlightControlSystem : public System_I
{
public:
    void update(const float delta) override;

private:
    glm::vec3 calculateCollective(
        const b3BodyId bodyId,
        float& collective,
        const glm::vec3 up,
        const float delta);

    glm::vec3 calculateCyclic(
        const b3BodyId bodyId,
        const glm::vec3 forward,
        const glm::vec3 right,
        const glm::vec3 up,
        const float delta);

    glm::vec3 calculateYaw(
        const b3BodyId bodyId,
        const glm::vec3 up,
        const float delta);

    glm::vec3 calculateSCAS(
        const b3BodyId bodyId,
        const glm::vec3 forward,
        const glm::vec3 right,
        const glm::vec3 up,
        const float delta);

    glm::vec3 calculateDrag(
        const b3BodyId bodyId,
        const glm::vec3 forward,
        const glm::vec3 right,
        const glm::vec3 up,
        const float delta);

    glm::vec3 damp(
    const glm::vec3& velocity,
    const glm::vec3& direction,
    const float damping,
    const float mass);
};