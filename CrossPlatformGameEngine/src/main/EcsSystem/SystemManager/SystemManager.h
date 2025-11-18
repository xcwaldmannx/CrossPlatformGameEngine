#pragma once

#include "../EcsCommon.h"
#include "System/System_I.h"

#include "../ComponentManager/ComponentManager.h"

#include <utility>
#include <memory>
#include <typeindex>

#include <unordered_set>
#include <unordered_map>

class SystemManager {
public:
	template<typename T, typename... Args>
	void registerSystem(
		Signature readSignature,
		Signature writeSignature,
		ComponentManager& componentManager,
		Args&&... args) 
	{
		std::type_index id = typeid(T);
		assert(mSystems.find(id) == mSystems.end() && "System already registered. Cannot register.");

		mSystems[id] = std::make_shared<T>(std::forward<Args>(args)...);
		mSystemReadSignatures[id] = readSignature;
		mSystemWriteSignatures[id] = writeSignature;
		mSystems[id]->mComponentManager = &componentManager;

		scheduleSystemUpdate<T>(readSignature, writeSignature);
	}

	void addEntity(EntityId entity, Signature signature) 
	{
		for (auto& pair : mSystems) {
			auto& systemId = pair.first;

			const auto& systemReadSig = mSystemReadSignatures[systemId];
			const auto& systemWriteSig = mSystemWriteSignatures[systemId];

			if ((systemReadSig & signature) == systemReadSig &&
				(systemWriteSig & signature) == systemWriteSig) {
				mSystems[systemId]->mEntities.emplace(entity);
			}
		}
	}

	void removeEntity(EntityId entity, Signature signature) 
	{
		for (auto& pair : mSystems) {
			auto& systemId = pair.first;

			const auto& systemReadSig = mSystemReadSignatures[systemId];
			const auto& systemWriteSig = mSystemWriteSignatures[systemId];

			if ((systemReadSig & signature) != systemReadSig ||
				(systemWriteSig & signature) != systemWriteSig) {
				mSystems[systemId]->mEntities.erase(entity);
			}
		}
	}

	void removeEntity(EntityId entity) 
	{
		for (auto& pair : mSystems) {
			auto& systemId = pair.first;

			mSystems[systemId]->mEntities.erase(entity);
		}
	}

	template<typename T>
	std::shared_ptr<T> getSystem()
	{
		std::type_index id = typeid(T);
		assert(mSystems.find(id) != mSystems.end() && "System is not registered. Cannot get.");

		return std::dynamic_pointer_cast<T>(mSystems[id]);
	}

	template<typename T>
	void updateSystem(float deltaTime)
	{
		std::type_index id = typeid(T);
		assert(mSystems.find(id) != mSystems.end() && "System is not registered. Cannot update.");

		mSystems[id]->update(deltaTime);
	}

	void updateAllSystems(float deltaTime)
	{
		int groupId = 0;
		for (auto& group : mSystemGroups)
		{
			std::cout << "Updating group " << groupId << ":" << std::endl;
			for (auto& systemId : group)
			{
				mSystems[systemId]->update(deltaTime);
			}
			std::cout << std::endl;

			groupId++;
		}
	}

private:
	/*
	* Schedules systems to be updated together if they
	* don't have conflicting updates.
	*/
	template<typename T>
	void scheduleSystemUpdate(Signature readSignature, Signature writeSignature)
	{
		std::type_index id = typeid(T);

		if (mSystemGroups.empty())
		{
			std::unordered_set<std::type_index> group;
			group.emplace(id);
			mSystemGroups.emplace_back(std::move(group));
		}
		else
		{
			bool foundGroup = false;

			for (auto& group : mSystemGroups)
			{

				bool isConflict = false;

				for (auto& systemId : group)
				{

					const auto& groupSystemReadSig = mSystemReadSignatures[systemId];
					const auto& groupSystemWriteSig = mSystemWriteSignatures[systemId];

					if ((writeSignature & groupSystemWriteSig).any() ||
					    (readSignature & groupSystemWriteSig).any()  ||
						(writeSignature & groupSystemReadSig).any())
					{
						// conflict found, try another group
						isConflict = true;
						break;
					}

				}

				// if there's no conflict, schedule the system for this group
				if (!isConflict)
				{
					group.emplace(id);
					foundGroup = true;
					break;
				}
			}

			// no group was found, create a new one
			if (!foundGroup)
			{
				std::unordered_set<std::type_index> group;
				group.emplace(id);
				mSystemGroups.emplace_back(std::move(group));
			}
		}
	}

private:
	std::unordered_map<std::type_index, std::shared_ptr<System_I>> mSystems;
	std::unordered_map<std::type_index, Signature> mSystemReadSignatures;
	std::unordered_map<std::type_index, Signature> mSystemWriteSignatures;

	std::vector<std::unordered_set<std::type_index>> mSystemGroups;
};