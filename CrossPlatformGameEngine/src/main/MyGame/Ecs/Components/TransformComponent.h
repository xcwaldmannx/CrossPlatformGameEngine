#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct TransformComponent
{
	glm::vec3 mPosition = glm::vec3(0.0);
	glm::quat mRotation = glm::quat();
	glm::vec3 mScale    = glm::vec3(1.0);
};