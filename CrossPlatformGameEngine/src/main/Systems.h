#pragma once

#include "EcsSystem/SystemManager/System/System_I.h"
#include "GraphicsPipeline/RenderComponents.h"
#include "Components.h"

#include <glm/gtx/quaternion.hpp>

struct TransformSystem : public System_I
{
	void update(float deltaTime)
	{
		for (EntityId e : mEntities)
		{
			auto& transform = mComponentManager->getComponent<TransformComponent>(e);
			// auto& position = transform.position;
			auto& rotation = transform.mRotation;
			auto& scale = transform.mScale;

			rotation += glm::vec3(0.0025, 0.0025, 0.0025) * deltaTime;
		}
	}
};

struct RenderSystem : public System_I
{
	void update(float deltaTime)
	{
		for (EntityId e : mEntities)
		{
			auto& transform = mComponentManager->getComponent<TransformComponent>(e);
			auto& model = mComponentManager->getComponent<ModelComponent>(e);
			auto& render = mComponentManager->getComponent<BasicRenderComponent>(e);

			glm::quat rotQuat = glm::quat(transform.mRotation); // Converts Euler (XYZ) to quaternion
			glm::mat4 rotMatrix = glm::toMat4(rotQuat);

			render.mTransform = glm::translate(glm::mat4(1.0f), transform.mPosition)
				* rotMatrix
				* glm::scale(glm::mat4(1.0f), transform.mScale);

			render.mTextureId = model.mTextureId;
		}
	}
};