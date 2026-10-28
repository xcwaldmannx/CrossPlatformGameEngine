#pragma once

#include <box3d/box3d.h>
#include <glm/glm.hpp>

enum PhysicsBodyType
{
    STATIC,
    DYNAMIC,
};

struct Box
{
    Box() : mLengths(1, 1, 1) {}

    Box(const float x, const float y, const float z) : mLengths(x, y, z) {}

    glm::vec3 mLengths;
};

struct Sphere
{
    Sphere() : mRadius(1.0f) {}
    Sphere(const float radius) : mRadius(radius) {}

    float mRadius;
};

struct Mesh
{
    std::vector<glm::vec3> mVertices;
    std::vector<int32_t> mIndices;
};

struct PhysicsComponent
{
    b3BodyId mId;
    PhysicsBodyType mBodyType = STATIC;
    float mDensity = 1.0f;
    std::variant<Box, Sphere, Mesh> mCollider = Box();
    glm::vec3 mAppliedForce = glm::vec3(0);
    glm::vec3 mAppliedTorque = glm::vec3(0);
};