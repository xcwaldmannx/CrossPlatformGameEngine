#pragma once

#include "TransformComponent.h"

#include <cstdint>

#include <glm/glm.hpp>

struct ModelComponent
{
	std::string mName;
	uint32_t mModelId    = 0;
	uint32_t mTextureId  = 0;
	uint32_t mIsSelected = 0;

	bool mIsHidden = false;

	std::vector<TransformComponent> mMeshTransforms;
};