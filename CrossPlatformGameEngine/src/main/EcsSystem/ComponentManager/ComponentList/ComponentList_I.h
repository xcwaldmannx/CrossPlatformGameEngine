#pragma once

#include "../../EcsCommon.h"

#include <string>

class ComponentList_I {
public:
	virtual ~ComponentList_I() = default;

	virtual void remove(EntityId entity) = 0;

	virtual std::string toString() = 0;
};