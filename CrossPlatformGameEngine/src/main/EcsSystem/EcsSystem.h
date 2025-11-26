#pragma once

#include "EntityManager/EntityManager.h"
#include "ComponentManager/ComponentManager.h"
#include "SystemManager/SystemManager.h"

#include <iostream>
#include <utility>

class EcsSystem
{
public:
	template<typename T>
	void registerComponent()
	{
		mComponentManager.registerComponent<T>();
		std::cout << "Registered new component!" << std::endl;
	}

	template<typename T, typename... Args>
	void registerSystem(Signature readSignature, Signature writeSignature, Args&&... args)
	{
		mSystemManager.registerSystem<T>(
			readSignature,
			writeSignature,
			mComponentManager,
			std::forward<Args>(args)...);
		std::cout << "Registered new system!" << std::endl;
	}

	EntityId addEntity()
	{
		assert(mEntityManager.count() < ENTITY_MAX && "Entity limit reached. Cannot add entity.");
		EntityId id = mEntityManager.addEntity();
		mEntityIdToSignature[id] = {};
		return id;
	}

	void removeEntity(EntityId entity)
	{
		assert(entity < ENTITY_MAX && "Entity outside limit. Cannot remove entity.");

		const Signature& signature = mEntityIdToSignature.at(entity);

		mSystemManager.removeEntity(entity);
		mComponentManager.removeComponents(entity, signature);
		mEntityManager.removeEntity(entity);
		mEntityIdToSignature.erase(entity);
	}

	template<typename T>
	void addComponent(EntityId entity, T&& component = {})
	{
		assert(entity < ENTITY_MAX && "Entity limit reached. Cannot add component.");

		mEntityIdToSignature.at(entity).set(mComponentManager.getComponentId<T>());
		Signature entitySignature = mEntityIdToSignature.at(entity);

		mComponentManager.addComponent<T>(entity, std::forward<T>(component));
		mSystemManager.addEntity(entity, entitySignature);
		mSystemManager.addDirtyEntity(entity, entitySignature);
	}

	template<typename T>
	void removeComponent(EntityId entity)
	{
		assert(entity < ENTITY_MAX && "Entity limit reached. Cannot remove components.");

		mComponentManager.removeComponent<T>(entity);
		mSystemManager.removeEntity(entity, mEntityIdToSignature.at(entity));
		mEntityIdToSignature.at(entity).reset();
	}

	template<typename T>
	T& getComponent(EntityId entity)
	{
		assert(entity < ENTITY_MAX && "Entity limit reached. Cannot get component.");
		mSystemManager.addDirtyEntity(entity, mEntityIdToSignature.at(entity));
		return mComponentManager.getComponent<T>(entity);
	}

	template<typename T>
	const T& getComponent(EntityId entity) const
	{
		assert(entity < ENTITY_MAX && "Entity limit reached. Cannot get component.");

		return mComponentManager.getComponent<T>(entity);
	}

	template<typename T>
	const std::shared_ptr<ComponentList<T>>& getComponentList()
	{
		return mComponentManager.getComponentList<T>();
	}

	template<typename... T>
	Signature getSignature()
	{
		Signature signature;
		(signature.set(mComponentManager.getComponentId<T>()), ...);
		return signature;
	}

	template<typename T>
	std::shared_ptr<T> getSystem()
	{
		return mSystemManager.getSystem<T>();
	}

	template<typename T>
	void updateSystem(float deltaTime)
	{
		mSystemManager.updateSystem<T>(deltaTime);
	}

	void updateAllSystems(float deltaTime)
	{
		mSystemManager.updateAllSystems(deltaTime);
	}

	std::string toString()
	{
		std::stringstream ss;

		ss << mComponentManager.toString();

		return ss.str();
	}

private:
	EntityManager mEntityManager;
	ComponentManager mComponentManager;
	SystemManager mSystemManager;
	std::unordered_map<EntityId, Signature> mEntityIdToSignature;

};