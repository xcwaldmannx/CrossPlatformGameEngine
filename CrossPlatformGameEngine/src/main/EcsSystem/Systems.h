#pragma once

#include "SystemManager/System/System_I.h"
#include "Components.h"

struct SystemA : public System_I {
	void update(const float delta) override {
		for (EntityId e : mEntities) {
			auto& a = mSystem->getComponent<ComponentA>(e);
			auto& b = mSystem->getComponent<ComponentB>(e);

			std::cout << "-- System A is updating entity " << e << std::endl;
		}
	}
};

struct SystemB : public System_I {
	void update(const float delta) override {
		for (EntityId e : mEntities) {
			const auto& b = mSystem->getComponent<ComponentB>(e);
			const auto& c = mSystem->getComponent<ComponentC>(e);

			auto& a = mSystem->getComponent<ComponentA>(e);
			auto& d = mSystem->getComponent<ComponentD>(e);

			std::cout << "-- System B is updating entity " << e << std::endl;
		}
	}
};

struct SystemC : public System_I {
	void update(const float delta) override {
		for (EntityId e : mEntities) {
			auto& b = mSystem->getComponent<ComponentB>(e);
			auto& c = mSystem->getComponent<ComponentC>(e);

			std::cout << "-- System C is updating entity " << e << std::endl;
		}
	}
};

struct SystemD : public System_I {
	void update(const float delta) override {
		for (EntityId e : mEntities) {
			auto& c = mSystem->getComponent<ComponentC>(e);
			auto& d = mSystem->getComponent<ComponentD>(e);

			std::cout << "-- System D is updating entity " << e << std::endl;
		}
	}
};
