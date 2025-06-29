#include "GraphicsPipeline/GraphicsPipeline.h"
#include "ModelManager/ModelManager.h"

#include "EcsSystem/EcsSystem.h"

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct EntityTest {
	Model model;
	glm::mat4 transform;
	std::unordered_map<std::string, glm::mat4> subTransforms;
};

void testModel() {
	ModelManager mm;
	mm.createModel("mesh1", "C:\\Users\\xcwal\\Documents\\Models\\testmodel.fbx");
	auto& model = mm.getModel("mesh1");

	glm::mat4 trans = glm::mat4(1.0f);

	EntityTest e1;
	e1.model = mm.getModel("mesh1");
	e1.transform = glm::translate(trans, glm::vec3(0, 0, 0));
	e1.subTransforms["x"] = glm::translate(glm::mat4(1.0f), glm::vec3(1, 0, 0));
	e1.subTransforms["y"] = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1, 0));
	e1.subTransforms["z"] = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1));
}

#include "EcsSystem/Systems.h"

void testEcs() {
	EcsSystem ecs;

	ecs.registerComponent<ComponentA>();
	ecs.registerComponent<ComponentB>();
	ecs.registerComponent<ComponentC>();
	ecs.registerComponent<ComponentD>();

	Signature readSignatureSystemA = ecs.getSignature<ComponentA>();
	Signature writeSignatureSystemA = ecs.getSignature<ComponentA, ComponentB>();
	ecs.registerSystem<SystemA>(readSignatureSystemA, writeSignatureSystemA);

	Signature readSignatureSystemB = ecs.getSignature<ComponentB, ComponentC>();
	Signature writeSignatureSystemB = ecs.getSignature<ComponentA, ComponentD>();
	ecs.registerSystem<SystemB>(readSignatureSystemB, writeSignatureSystemB);

	Signature readSignatureSystemC = ecs.getSignature<ComponentC>();
	Signature writeSignatureSystemC = ecs.getSignature<ComponentB, ComponentC>();
	ecs.registerSystem<SystemC>(readSignatureSystemC, writeSignatureSystemC);

	Signature readSignatureSystemD = ecs.getSignature<ComponentD>();
	Signature writeSignatureSystemD = ecs.getSignature<ComponentC, ComponentD>();
	ecs.registerSystem<SystemD>(readSignatureSystemD, writeSignatureSystemD);


	ecs.updateSystem<SystemA>();
	ecs.updateSystem<SystemB>();

	auto e1 = ecs.addEntity();
	ecs.addComponent<ComponentA>(e1, {});
	ecs.addComponent<ComponentB>(e1, {});

	auto e2 = ecs.addEntity();
	ecs.addComponent<ComponentB>(e2, {});
	ecs.addComponent<ComponentC>(e2, {});

	auto e3 = ecs.addEntity();
	ecs.addComponent<ComponentC>(e3, {});
	ecs.addComponent<ComponentD>(e3, {});

	auto e4 = ecs.addEntity();
	ecs.addComponent<ComponentA>(e4, {});
	ecs.addComponent<ComponentB>(e4, {});
	ecs.addComponent<ComponentC>(e4, {});
	ecs.addComponent<ComponentD>(e4, {});

	ecs.updateAllSystems();

}

void testGraphics() {
	GraphicsPipeline gp;
	gp.create();
	gp.renderLoop();
	gp.destroy();
}

int main() {
	// testEcs();
	testGraphics();
}