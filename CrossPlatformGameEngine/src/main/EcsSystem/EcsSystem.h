#pragma once

#include "EntityManager/EntityManager.h"
#include "ComponentManager/ComponentManager.h"
#include "SystemManager/SystemManager.h"

#include <iostream>

class EcsSystem {
public:
	template<typename T>
	void registerComponent()
	{
		mComponentManager.registerComponent<T>();
		std::cout << "Registered new component!" << std::endl;
	}

	template<typename T>
	void registerSystem(Signature readSignature, Signature writeSignature)
	{
		mSystemManager.registerSystem<T>(readSignature, writeSignature, mComponentManager);
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
		mComponentManager.addComponent<T>(entity, std::forward<T>(component));
		mSystemManager.addEntity(entity, mEntityIdToSignature.at(entity));
	}

	template<typename T>
	void removeComponent(EntityId entity)
	{
		assert(entity < ENTITY_MAX && "Entity limit reached. Cannot remove components.");

		mComponentManager.removeComponent<T>(entity);
		mEntityIdToSignature.at(entity).reset();
		mSystemManager.removeEntity(entity, mEntityIdToSignature.at(entity));
	}

	template<typename T>
	T& getComponent(EntityId entity)
	{
		assert(entity < ENTITY_MAX && "Entity limit reached. Cannot get component.");

		return mComponentManager.getComponent<T>(entity);
	}

	template<typename T>
	std::shared_ptr<ComponentList<T>> getComponentList()
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