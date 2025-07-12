#pragma once

#include "../ModelManager/ModelManager.h"

#include <vector>

struct ascen::Buffer;
struct ascen::Texture;

// In the future, avoid the array of structures, and use a structure of arrays
// Also, this is more of an object than a mesh. Don't get confused.

struct Entity {
	Model mModel;
	glm::mat4 mTransform;
	int mTextureIndex;
};