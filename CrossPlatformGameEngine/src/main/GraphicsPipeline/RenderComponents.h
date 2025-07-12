#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Contains required data for rendering an entity/object
struct BasicRenderComponent
{
	glm::mat4 mTransform;
	int mTextureId;
};