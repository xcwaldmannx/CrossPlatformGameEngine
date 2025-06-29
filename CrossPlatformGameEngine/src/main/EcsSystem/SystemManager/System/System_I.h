#pragma once

#include "../../EcsCommon.h"
#include "../../ComponentManager/ComponentManager.h"

#include <memory>
#include <unordered_set>

class System_I {
public:
	virtual ~System_I() = default;

	virtual void update() = 0;
	
public:
	std::unordered_set<EntityId> mEntities;
	ComponentManager* mComponentManager = nullptr;
};