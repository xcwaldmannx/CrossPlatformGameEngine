#pragma once

#include <box3d/box3d.h>
#include <glm/glm.hpp>

enum PhysicsBodyType
{
    DYNAMIC
};

struct PhysicsBodyComponent
{
    b3BodyId mId;
    PhysicsBodyType mBodyType = DYNAMIC;
    float mDensity = 1.0f;
    glm::vec3 mAppliedForce = glm::vec3(0);
    glm::vec3 mAppliedTorque = glm::vec3(0);
};