#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct CameraComponent
{
    glm::vec3 mPosition;
    glm::quat mRotation = { 1, 0, 0, 0 };
    glm::vec3 mOffsetPosition;
    glm::quat mOffsetRotation = { 1, 0, 0, 0 };
    bool mIsActive = false;
};