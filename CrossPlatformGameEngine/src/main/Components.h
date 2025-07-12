#pragma once

#include "ModelManager/ModelManager.h"

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct TransformComponent
{
	glm::vec3 mPosition;
	glm::vec3 mRotation;
	glm::vec3 mScale;
};

struct ModelComponent
{
	Model mModel;
	int mTextureId;
};