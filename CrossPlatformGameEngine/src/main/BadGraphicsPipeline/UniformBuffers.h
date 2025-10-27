#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct UniformBufferObject {
	glm::mat4 mView;
	glm::mat4 mProj;
};

struct Camera {
	glm::mat4 mView;
	glm::mat4 mProj;
};