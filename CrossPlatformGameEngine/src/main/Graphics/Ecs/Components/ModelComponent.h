#pragma once

#include <cstdint>

#include <glm/glm.hpp>

struct ModelComponent
{
	uint32_t mModelId   = 0;
	uint32_t mTextureId = 0;

	bool mIsHidden = false;
};