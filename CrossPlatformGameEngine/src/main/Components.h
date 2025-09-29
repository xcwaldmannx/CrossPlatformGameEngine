#pragma once

#include "ModelManager/ModelManager.h"

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Render Components

struct TransformComponent
{
	glm::vec3 mPosition = glm::vec3(0);
	glm::vec3 mRotation = glm::vec3(0);
	glm::vec3 mScale    = glm::vec3(0);
};

struct ModelComponent
{
	Model mModel;
	int mTextureId = 0;
};

// Physics Components

enum Collider
{
	AABB,
	SPHERE,
	MESH,
};

struct ColliderComponent
{
	Collider mCollider = AABB;
	bool mIsCollidable = true;
};

struct RigidbodyComponent
{
	void setMass(float m) {
		mMass = m;
		mInvMass = (m > 0.0f) ? 1.0f / m : 0.0f;
	}

	void addForce(const glm::vec3& f) 
	{
		mForceAccum += f;
	}

	void applyImpulse(const glm::vec3& J)
	{ 
		mVelocity += J * mInvMass;
	}

	void step(glm::vec3& position, float dt)
	{
		if (dt <= 0.0f || mInvMass == 0.0f)
		{
			mForceAccum = glm::vec3(0.0f);
			return;
		}

		const glm::vec3 gravity(0.0f, 0.0f, -9.81f);

		// a = gravity + (sum of forces) * invMass
		glm::vec3 accel = gravity + mForceAccum * mInvMass;

		mVelocity += accel * dt;
		position += mVelocity * dt;
		mForceAccum = glm::vec3(0.0f);
	}

	float mMass = 1.0f;
	float mInvMass = 1.0f;
	glm::vec3 mVelocity = glm::vec3(0);
	glm::vec3 mForceAccum = glm::vec3(0);
};