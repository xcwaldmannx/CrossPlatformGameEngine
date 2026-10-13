#pragma once

#include <box3d/box3d.h>
#include <glm/glm.hpp>

enum PhysicsBodyType
{
    DYNAMIC
};

struct PhysicsBodyComponent
{
    PhysicsBodyType mBodyType = DYNAMIC;
    float mDensity = 1;
    glm::vec3 mAppliedForce = glm::vec3(0);
};