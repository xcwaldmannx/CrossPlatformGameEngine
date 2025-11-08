#pragma once

#include "ComponentList/ComponentList.h"
#include "../EcsCommon.h"

#include <memory>
#include <typeindex>
#include <cassert>

#include <vector>
#include <unordered_map>

class ComponentManager {
public:
	ComponentManager() : mRegisteredComponentCount(0) {};

	template<typename T>
	void registerComponent() {
		std::type_index component = typeid(T);
		assert(mComponentLists.find(component) == mComponentLists.end() &&
			"Component already registered. Cannot register.");

		mComponentToComponentId.emplace(component, mRegisteredComponentCount);
		mComponentIdToComponent.emplace(mRegisteredComponentCount, component);
		mComponentLists[component] = std::make_unique<ComponentList<T>>();

		mRegisteredComponentCount++;
	}

	template<typename T>
	void addComponent(EntityId entity, T&& component) {
		std::type_index comp = typeid(T);
		auto it = mComponentLists.find(comp);
		assert(it != mComponentLists.end() && "Component is not registered. Cannot add.");

		ComponentList<T>* list = static_cast<ComponentList<T>*>(it->second.get());
		list->add(entity, std::forward<T>(component));
	}

	template<typename T>
	void removeComponent(EntityId entity) {
		std::type_index component = typeid(T);
		auto it = mComponentLists.find(component);
		assert(it != mComponentLists.end() && "Component is not registered. Cannot remove.");

		ComponentList<T>* list = static_cast<ComponentList<T>*>(it->second.get());
		list->remove(entity);
	}

	void removeComponents(EntityId entity, const Signature& signature) {
		for (int i = 0; i < signature.size(); i++) {
			if (signature.test(i)) {
				const std::type_index& component = mComponentIdToComponent.at(i);
				auto it = mComponentLists.find(component);
				assert(it != mComponentLists.end() && "Component is not registered. Cannot remove.");

				it->second->remove(entity);
			}
		}
	}

	template<typename T>
	T& getComponent(EntityId entity) {
		std::type_index component = typeid(T);
		auto it = mComponentLists.find(component);
		assert(it != mComponentLists.end() && "Component is not registered. Cannot get.");

		ComponentList<T>* list = static_cast<ComponentList<T>*>(it->second.get());

		return list->get(entity);
	}

	template<typename T>
	const std::shared_ptr<ComponentList<T>>& getComponentList() {
		std::type_index type = typeid(T);
		auto it = mComponentLists.find(type);
		assert(it != mComponentLists.end() && "Component is not registered. Cannot get list.");

		return std::dynamic_pointer_cast<ComponentList<T>>(it->second);
	}

	template<typename T>
	uint32_t getComponentId() {
		std::type_index component = typeid(T);
		assert(mComponentToComponentId.find(component) != mComponentToComponentId.end() &&
			"Component is not registered. Cannot get id.");

		return mComponentToComponentId[component];
	}

	std::string toString() {
		std::stringstream ss;

		for (const auto& pair : mComponentLists) {
			ss << pair.second->toString();
		}

		return ss.str();
	}

private:
	uint32_t mRegisteredComponentCount;
	std::unordered_map<std::type_index, uint32_t> mComponentToComponentId;
	std::unordered_map<uint32_t, std::type_index> mComponentIdToComponent;
	std::unordered_map<std::type_index, std::shared_ptr<ComponentList_I>> mComponentLists;

};