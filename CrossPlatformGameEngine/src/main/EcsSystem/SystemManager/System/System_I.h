#pragma once

#include "../../EcsCommon.h"
#include "../../ComponentManager/ComponentManager.h"

#include <memory>
#include <unordered_set>

class System_I
{
public:
	virtual ~System_I() = default;

	virtual void update(float deltaTime) = 0;
	
public:
	std::unordered_set<EntityId> mEntities;
	std::unordered_set<EntityId> mDirtyEntities;
	ComponentManager* mComponentManager = nullptr;
};