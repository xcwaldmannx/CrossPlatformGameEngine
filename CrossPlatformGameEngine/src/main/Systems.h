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
			auto& rotation = transform.mRotation;

			rotation += glm::vec3(0, 0, 0.5f) * deltaTime;
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

			render.mTransform = 
				(glm::translate(glm::mat4(1.0f), transform.mPosition)
				* rotMatrix
				* glm::scale(glm::mat4(1.0f), transform.mScale));

			render.mTextureId = model.mTextureId;
		}
	}
};

struct PhysicsSystem : public System_I
{
	void update(float deltaTime)
	{
		for (EntityId e : mEntities)
		{
			auto& collider = mComponentManager->getComponent<ColliderComponent>(e);
			auto& transform = mComponentManager->getComponent<TransformComponent>(e);
			auto& rigidbody = mComponentManager->getComponent<RigidbodyComponent>(e);

			if (collider.mIsCollidable)
			{
				glm::vec3 gravity(0.0f, 0.0f, -9.81f);
				rigidbody.addForce(rigidbody.mMass * gravity);

				if (transform.mPosition.z <= -1.0f && 
					abs(rigidbody.mVelocity.z) <= 0.01f &&
					abs(rigidbody.mForceAccum.z) <= 0.01f)
				{
					transform.mPosition.z = -1.0f;
					rigidbody.mVelocity.z = 0.0f;
					rigidbody.mForceAccum.z = 0.0f;
				}
				else
				{
					rigidbody.step(transform.mPosition, deltaTime);

					if (transform.mPosition.z <= -1.0f)
					{
						transform.mPosition.z = -1.0f;
						rigidbody.mVelocity *= -0.85f;
					}
				}
			}
		}
	}
};