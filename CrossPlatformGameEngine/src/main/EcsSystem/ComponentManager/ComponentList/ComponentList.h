#pragma once

#include "ComponentList_I.h"

#include <iostream>
#include <cassert>

#include <sstream>
#include <vector>
#include <unordered_map>
#include <stack>

template<typename T>
class ComponentList : public ComponentList_I {
public:
	ComponentList() : mComponentCount(0) {
		mComponents.reserve(ENTITY_MAX);
	}

	void add(EntityId entity, T&& component) {
		assert(!has(entity) && "Entity already has component. Cannot add.");

		assert((mComponentCount >= ENTITY_MIN && mComponentCount < ENTITY_MAX) &&
			"Component count not in range. Cannot add.");

		mEntityToComponentIdx[entity] = mComponentCount;
		mComponentIdxToEntity[mComponentCount] = entity;

		mComponents.emplace_back(std::forward<T>(component));

		mComponentCount++;
	}

	void remove(EntityId entity) override {
		assert(has(entity) && "Entity does not have component. Cannot remove.");

		uint32_t index = mEntityToComponentIdx[entity];
		uint32_t lastIndex = mComponentCount - 1;

		if (index != lastIndex) { // move the last component to the space being removed to keep contiguous
			mComponents[index] = std::move(mComponents[lastIndex]);

			// update the mappings
			EntityId entityLastIndex = mComponentIdxToEntity[lastIndex];
			mComponentIdxToEntity[index] = entityLastIndex;
			mEntityToComponentIdx[entityLastIndex] = index;
		}

		mEntityToComponentIdx.erase(entity);
		mComponentIdxToEntity.erase(lastIndex);

		mComponents[lastIndex] = T{};
		mComponents.pop_back();
		mComponentCount--;
	}

	T& get(EntityId entity)
	{
		assert(has(entity) && "Entity does not have component. Cannot get.");

		return mComponents.at(mEntityToComponentIdx.at(entity));
	}

	bool has(EntityId entity) const {
		return mEntityToComponentIdx.find(entity) != mEntityToComponentIdx.end();
	}

	std::vector<T>& toList()
	{
		return mComponents;
	}

	std::string toString() override {
		std::stringstream ss;

		ss << "EntityToComponentIdx\n";
		for (const auto& pair : mEntityToComponentIdx) {
			ss << "{" << pair.first << ", " << pair.second << "} , ";
		}

		ss << "\nComponentIdxToEntity\n";
		for (const auto& pair : mComponentIdxToEntity) {
			ss << "{" << pair.first << ", " << pair.second << "} , ";
		}
		ss << "\n";

		return ss.str();
	}

private:
	std::unordered_map<EntityId, uint32_t> mEntityToComponentIdx;
	std::unordered_map<uint32_t, EntityId> mComponentIdxToEntity;
	std::vector<T> mComponents;
	uint32_t mComponentCount;
};