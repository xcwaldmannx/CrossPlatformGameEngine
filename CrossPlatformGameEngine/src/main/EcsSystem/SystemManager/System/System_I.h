#pragma once

#include "../../EcsCommon.h"

#include <memory>
#include <unordered_set>

class EcsSystem;

class System_I
{
public:
	virtual ~System_I() = default;

	virtual void update(const float delta) = 0;
	
public:
	std::unordered_set<EntityId> mEntities;
	std::unordered_set<EntityId> mDirtyEntities;
	EcsSystem* mSystem = nullptr;
};