//#include "GraphicsPipeline/GraphicsPipeline.h"
#include "MyGraphicsPipeline/MyGraphicsPipeline.h"
#include "ModelManager/ModelManager.h"

#include "EcsSystem/EcsSystem.h"
#include"Systems.h"
#include "Components.h"

#include "Utility/QuadTree/QuadTree.h"
#include "Utility/OctTree/OctTree.h"

#include <chrono>
#include <thread>

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


	ecs.updateSystem<SystemA>(0);
	ecs.updateSystem<SystemB>(0);

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

	ecs.updateAllSystems(0);

}

//void testGraphics() {
//	ModelManager modelManager;
//	modelManager.createModel("model_00", "C:\\Users\\xcwal\\Documents\\Models\\testmodel0.fbx");
//	modelManager.createModel("model_01", "C:\\Users\\xcwal\\Documents\\Models\\testmodel.fbx");
//	modelManager.createModel("model_02", "C:\\Users\\xcwal\\Documents\\Models\\testmodel_1.fbx");
//	modelManager.createModel("model_04", "C:\\Users\\xcwal\\Documents\\Models\\testhuman1.fbx");
//	modelManager.createModel("model_03", "C:\\Users\\xcwal\\Documents\\Models\\testmodel_2.fbx");
//	modelManager.createModel("shell", "C:\\Users\\xcwal\\Documents\\Models\\shell.fbx");
//	modelManager.createModel("snail", "C:\\Users\\xcwal\\Documents\\Models\\snail.fbx");
//
//	// TODO: Still need a TextureManager to map texture names to ids for shader texture arrays
//	std::vector<const char*> textureFilepaths =
//	{
//		"C:\\Users\\xcwal\\OneDrive\\Desktop\\testimg1.png",
//		"C:\\Users\\xcwal\\OneDrive\\Desktop\\testimg2.png",
//		//"C:\\Users\\xcwal\\Documents\\Models\\Ernest_body_base color.png",
//	};
//
//	EcsSystem ecs;
//
//	ecs.registerComponent<BasicRenderComponent>();
//	ecs.registerComponent<TransformComponent>();
//	ecs.registerComponent<ModelComponent>();
//	ecs.registerComponent<ColliderComponent>();
//	ecs.registerComponent<RigidbodyComponent>();
//
//	Signature rSigTransform = ecs.getSignature<TransformComponent>();
//	Signature wSigTransform = ecs.getSignature<TransformComponent>();
//	ecs.registerSystem<TransformSystem>(rSigTransform, wSigTransform);
//
//	Signature rSigRender = ecs.getSignature<TransformComponent, ModelComponent>();
//	Signature wSigRender = ecs.getSignature<BasicRenderComponent>();
//	ecs.registerSystem<RenderSystem>(rSigRender, wSigRender);
//
//	Signature rSigPhysics = ecs.getSignature<ColliderComponent>();
//	Signature wSigPhysics = ecs.getSignature<TransformComponent, RigidbodyComponent>();
//	ecs.registerSystem<PhysicsSystem>(rSigPhysics, wSigPhysics);
//
//	const Model& model = modelManager.getModel("model_04");
//	int model_id = modelManager.getModelId("model_04");
//
//	int scale = 16;
//	int step = 2;
//
//	OctTreeRegion otr{ { -scale, -scale, -scale }, scale * 2 };
//	OctTree<EntityId> otn(otr, 2);
//
//
//	glm::vec3 position(0, 0, 0);
//
//	auto e = ecs.addEntity();
//	ecs.addComponent<BasicRenderComponent>(e, { glm::mat4(), model_id, 0 });
//	ecs.addComponent<TransformComponent>(e, { position, glm::vec3(0, 0, 0), glm::vec3(10, 10, 10) });
//	ecs.addComponent<ModelComponent>(e, { model, 0 });
//	ecs.addComponent<ColliderComponent>(e, { AABB, true });
//	ecs.addComponent<RigidbodyComponent>(e);
//
//	ecs.getComponent<RigidbodyComponent>(e).setMass(30.0f);
//	ecs.getComponent<RigidbodyComponent>(e).applyImpulse(glm::vec3{ 0, 0, 50 });
//
//	GraphicsPipeline gp;
//	gp.setModels(modelManager);
//	gp.setTextures(textureFilepaths);
//	gp.create();
//
//	std::chrono::time_point startTime = std::chrono::high_resolution_clock::now();
//	std::chrono::time_point lastFrameTime = startTime;
//	std::chrono::milliseconds pollInterval(100);
//	std::chrono::time_point pollEventsTime = startTime + pollInterval;
//
//	float timePassed = 0;
//
//	while (gp.isRunning())
//	{
//		std::chrono::time_point currentTime = std::chrono::high_resolution_clock::now();
//		std::chrono::duration<double, std::milli> delta = currentTime - lastFrameTime;
//		float deltaTimeSeconds = delta.count() * 0.001f;
//		lastFrameTime = currentTime;
//
//		// TODO: this should occur on a seprate thread
//		if (currentTime >= pollEventsTime)
//		{
//			gp.pollEvents();
//			pollEventsTime = currentTime + pollInterval;
//		}
//
//		ecs.updateSystem<PhysicsSystem>(deltaTimeSeconds);
//		ecs.updateSystem<TransformSystem>(deltaTimeSeconds);
//		ecs.updateSystem<RenderSystem>(deltaTimeSeconds);
//
//
//		std::vector<BasicRenderComponent> renderComponents;
//
//		renderComponents = ecs.getComponentList<BasicRenderComponent>()->toList();
//
//		if (!renderComponents.empty()) gp.submit(renderComponents);
//		
//		gp.render();
//
//		static float frameSum = 0.0f;
//		static int frameCount = 0;
//		frameSum += deltaTimeSeconds;
//		frameCount++;
//		if (frameSum >= 1.0f) {
//			std::cout << "FPS: " << frameCount << " (" << ((frameSum / frameCount) * 1000.0f) << " ms)\n";
//			frameSum = 0.0f;
//			frameCount = 0;
//		}
//
//		timePassed += deltaTimeSeconds;
//	}
//
//	gp.destroy();
//}
std::atomic<bool> running = true;

void drawLoop(MyGraphicsPipeline* gp)
{
	while (running)
	{
		if (gp->isResized())
		{
			gp->resize();
		}

		gp->drawFrame();
	}
}

void testGraphics2()
{
	WindowManager wm;
	wm.init();

	MyGraphicsPipeline gp(wm);

	gp.create();

	std::thread renderThread(drawLoop, &gp);
		
	while (wm.isRunning())
	{
		wm.pollEvents();
	}

	running = false;
	renderThread.join();

	gp.destroy();
	wm.destroy();

}

int main() {
	// testEcs();
	testGraphics2();
}
