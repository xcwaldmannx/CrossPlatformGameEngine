#pragma once

#include "SystemManager/System/System_I.h"
#include "Components.h"

struct SystemA : public System_I {
	void update() {
		for (EntityId e : mEntities) {
			auto& a = mComponentManager->getComponent<ComponentA>(e);
			auto& b = mComponentManager->getComponent<ComponentB>(e);

			std::cout << "-- System A is updating entity " << e << std::endl;
		}
	}
};

struct SystemB : public System_I {
	void update() {
		for (EntityId e : mEntities) {
			const auto& b = mComponentManager->getComponent<ComponentB>(e);
			const auto& c = mComponentManager->getComponent<ComponentC>(e);

			auto& a = mComponentManager->getComponent<ComponentA>(e);
			auto& d = mComponentManager->getComponent<ComponentD>(e);

			std::cout << "-- System B is updating entity " << e << std::endl;
		}
	}
};

struct SystemC : public System_I {
	void update() {
		for (EntityId e : mEntities) {
			auto& b = mComponentManager->getComponent<ComponentB>(e);
			auto& c = mComponentManager->getComponent<ComponentC>(e);

			std::cout << "-- System C is updating entity " << e << std::endl;
		}
	}
};

struct SystemD : public System_I {
	void update() {
		for (EntityId e : mEntities) {
			auto& c = mComponentManager->getComponent<ComponentC>(e);
			auto& d = mComponentManager->getComponent<ComponentD>(e);

			std::cout << "-- System D is updating entity " << e << std::endl;
		}
	}
};
