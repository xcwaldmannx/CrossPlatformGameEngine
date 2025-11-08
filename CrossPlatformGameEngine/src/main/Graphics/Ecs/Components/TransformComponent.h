#pragma once

#include <glm/glm.hpp>

struct TransformComponent
{
	glm::vec3 mPosition = glm::vec3(0.0);
	glm::vec3 mRotation = glm::vec3(0.0);
	glm::vec3 mScale    = glm::vec3(0.0);
};