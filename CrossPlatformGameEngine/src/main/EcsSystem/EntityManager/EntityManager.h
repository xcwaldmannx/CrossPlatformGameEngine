#pragma once

#include "../EcsCommon.h"

#include <cassert>

#include <array>
#include <queue>

class EntityManager
{
public:
	EntityManager() : mEntityCount(0)
	{
		for (EntityId i = 0; i < ENTITY_MAX; i++)
		{
			mAvailableEntityIds.push(i);
		}
	};

	EntityId addEntity()
	{
		assert((mEntityCount >= ENTITY_MIN && mEntityCount < ENTITY_MAX) &&
			!mAvailableEntityIds.empty() &&
			"Max entities reached. Cannot add.");

		EntityId id = mAvailableEntityIds.front();
		mAvailableEntityIds.pop();
		mEntityCount++;

		return id;
	}

	void removeEntity(EntityId entity)
	{
		assert(mEntityCount > ENTITY_MIN && "No entities to remove. Cannot remove.");

		mAvailableEntityIds.push(entity);
		mEntityCount--;
	}

	uint32_t count() const
	{
		return mEntityCount;
	}

private:
	uint32_t mEntityCount;
	std::array<EntityId, ENTITY_MAX> mEntities;
	std::queue<EntityId> mAvailableEntityIds;
};