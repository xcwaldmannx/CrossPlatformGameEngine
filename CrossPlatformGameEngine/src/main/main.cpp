//#include "GraphicsPipeline/GraphicsPipeline.h"
#include "MyGraphicsPipeline/MyGraphicsPipeline.h"
// #include "ModelManager/ModelManager.h"

#include"MyGraphicsPipeline/GraphicsPipeline_I.h"
#include "MyGraphicsPipeline/Test/TestGraphicsPipeline.h"

#include "EcsSystem/EcsSystem.h"
//#include"Systems.h"
//#include "Components.h"

#include "Utility/QuadTree/QuadTree.h"
#include "Utility/OctTree/OctTree.h"

#include <chrono>
#include <thread>

#include <string>

#include <Mass.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//struct EntityTest {
//	Model model;
//	glm::mat4 transform;
//	std::unordered_map<std::string, glm::mat4> subTransforms;
//};
//
//void testModel() {
//	ModelManager mm;
//	mm.createModel("mesh1", "C:\\Users\\xcwal\\Documents\\Models\\testmodel.fbx");
//	auto& model = mm.getModel("mesh1");
//
//	glm::mat4 trans = glm::mat4(1.0f);
//
//	EntityTest e1;
//	e1.model = mm.getModel("mesh1");
//	e1.transform = glm::translate(trans, glm::vec3(0, 0, 0));
//	e1.subTransforms["x"] = glm::translate(glm::mat4(1.0f), glm::vec3(1, 0, 0));
//	e1.subTransforms["y"] = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1, 0));
//	e1.subTransforms["z"] = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1));
//}

//#include "EcsSystem/Systems.h"

//void testEcs() {
//	EcsSystem ecs;
//
//	ecs.registerComponent<ComponentA>();
//	ecs.registerComponent<ComponentB>();
//	ecs.registerComponent<ComponentC>();
//	ecs.registerComponent<ComponentD>();
//
//	Signature readSignatureSystemA = ecs.getSignature<ComponentA>();
//	Signature writeSignatureSystemA = ecs.getSignature<ComponentA, ComponentB>();
//	ecs.registerSystem<SystemA>(readSignatureSystemA, writeSignatureSystemA);
//
//	Signature readSignatureSystemB = ecs.getSignature<ComponentB, ComponentC>();
//	Signature writeSignatureSystemB = ecs.getSignature<ComponentA, ComponentD>();
//	ecs.registerSystem<SystemB>(readSignatureSystemB, writeSignatureSystemB);
//
//	Signature readSignatureSystemC = ecs.getSignature<ComponentC>();
//	Signature writeSignatureSystemC = ecs.getSignature<ComponentB, ComponentC>();
//	ecs.registerSystem<SystemC>(readSignatureSystemC, writeSignatureSystemC);
//
//	Signature readSignatureSystemD = ecs.getSignature<ComponentD>();
//	Signature writeSignatureSystemD = ecs.getSignature<ComponentC, ComponentD>();
//	ecs.registerSystem<SystemD>(readSignatureSystemD, writeSignatureSystemD);
//
//
//	ecs.updateSystem<SystemA>(0);
//	ecs.updateSystem<SystemB>(0);
//
//	auto e1 = ecs.addEntity();
//	ecs.addComponent<ComponentA>(e1, {});
//	ecs.addComponent<ComponentB>(e1, {});
//
//	auto e2 = ecs.addEntity();
//	ecs.addComponent<ComponentB>(e2, {});
//	ecs.addComponent<ComponentC>(e2, {});
//
//	auto e3 = ecs.addEntity();
//	ecs.addComponent<ComponentC>(e3, {});
//	ecs.addComponent<ComponentD>(e3, {});
//
//	auto e4 = ecs.addEntity();
//	ecs.addComponent<ComponentA>(e4, {});
//	ecs.addComponent<ComponentB>(e4, {});
//	ecs.addComponent<ComponentC>(e4, {});
//	ecs.addComponent<ComponentD>(e4, {});
//
//	ecs.updateAllSystems(0);
//
//}

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

void drawLoop(ascen::GraphicsPipeline_I* gp)
{
	while (running)
	{
		if (gp->isResized())
		{
			gp->resize();
		}

		gp->drawFrame(0);
	}
}

//void testGraphics2()
//{
//	WindowManager wm;
//	wm.init();
//
//	MyGraphicsPipeline gp(wm);
//
//	gp.create();
//
//	std::thread renderThread(drawLoop, &gp);
//		
//	while (wm.isRunning())
//	{
//		wm.pollEvents();
//	}
//
//	running = false;
//	renderThread.join();
//
//	gp.destroy();
//	wm.destroy();
//
//}

#include "Graphics/Ecs/Components/ModelComponent.h"
#include "Graphics/Ecs/Components/TransformComponent.h"

#include "Graphics/Ecs/Systems/RenderSystem.h"

EcsSystem mEcs;

std::unordered_map<uint32_t, ModelData> mModelInfos;

void initECS(EcsSystem& ecs)
{
	ecs.registerComponent<TransformComponent>();
	ecs.registerComponent<ModelComponent>();

	Signature readSig = ecs.getSignature<TransformComponent, ModelComponent>();
	Signature writeSig = ecs.getSignature<ModelComponent>();
	ecs.registerSystem<RenderSystem>(readSig, writeSig, &mModelInfos);
}

enum MyModels : uint32_t
{
	NONE     = 0,
	PRISM    = 1,
	SHAPES   = 2,
	WINDMILL = 3,
};

std::vector<float> mVertices;
std::vector<uint32_t> mIndices;
std::vector<float> mTransforms;

void initModels()
{
	mass::Configuration config{};
	config.mVertexLayout.mAttributes =
	{
		{ 3, sizeof(float), 0 },
		{ 3, sizeof(float), sizeof(float) * 3 },
		{ 2, sizeof(float), sizeof(float) * 6 },
	};
	config.mHasNormals = true;
	config.mHasTexCoords = true;
	config.mHasTransforms = true;

	std::vector<std::pair<uint32_t, std::string>> modelFilepaths =
	{
		{ PRISM,    "res/models/prism.model"    },
		{ SHAPES,   "res/models/shapes.model"   },
		{ WINDMILL, "res/models/windmill.model" },
	};

	uint32_t globalVertexOffset = 0;
	uint32_t globalIndexOffset = 0;
	uint32_t globalTransformOffset = 0;

	for (const auto& path : modelFilepaths)
	{
		mass::ModelLayout modelLayout = mass::deserialize(config, path.second);

		uint32_t floatsPerVertex = modelLayout.mVertexLayout.mStride / sizeof(float);

		assert(modelLayout.mVertices.size() % (modelLayout.mVertexLayout.mStride / sizeof(float)) == 0);
		assert(modelLayout.mTransforms.size() % 16 == 0);

		ModelData info{};

		for (const auto& mesh : modelLayout.mMeshLayouts)
		{
			info.mMeshCount++;
			info.mVertexOffsets.push_back(globalVertexOffset);
			info.mIndexOffsets.push_back(globalIndexOffset + mesh.mIndexOffset);
			info.mIndexCounts.push_back(mesh.mIndexCount);
			info.mTransformOffsets.push_back(globalTransformOffset + mesh.mTransformOffset);
		}

		mModelInfos.emplace(path.first, std::move(info));

		mVertices.insert(mVertices.end(), modelLayout.mVertices.begin(), modelLayout.mVertices.end());
		mIndices.insert(mIndices.end(), modelLayout.mIndices.begin(), modelLayout.mIndices.end());
		mTransforms.insert(mTransforms.end(), modelLayout.mTransforms.begin(), modelLayout.mTransforms.end());

		globalVertexOffset = mVertices.size() / floatsPerVertex;
		globalIndexOffset = mIndices.size();
		globalTransformOffset = mTransforms.size() / 16;
	}
}

void testECSandPerMeshRender()
{
	initECS(mEcs);



	auto e0 = mEcs.addEntity();

	TransformComponent t0{};
	t0.mPosition = { -6, 0, 0 };
	t0.mRotation = {  0, 0, 0 };
	t0.mScale    = {  1, 1, 1 };

	ModelComponent m0{};
	m0.mModelId = PRISM;
	m0.mTextureId = 0;
	m0.mIsHidden = false;

	mEcs.addComponent<TransformComponent>(e0, std::move(t0));
	mEcs.addComponent<ModelComponent>(e0, std::move(m0));



	auto e1 = mEcs.addEntity();

	TransformComponent t1{};
	t1.mPosition = { 6, 0, 0 };
	t1.mRotation = { 0, 0, 0 };
	t1.mScale    = { 3, 3, 3 };

	ModelComponent m1{};
	m1.mModelId = SHAPES;
	m1.mTextureId = 0;
	m1.mIsHidden = false;

	mEcs.addComponent<TransformComponent>(e1, std::move(t1));
	mEcs.addComponent<ModelComponent>(e1, std::move(m1));

	auto e2 = mEcs.addEntity();

	TransformComponent t2{};
	t2.mPosition = { 0, 0, 0 };
	t2.mRotation = { 0, 0, 0 };
	t2.mScale    = { 1, 0, 1 };

	ModelComponent m2{};
	m2.mModelId = WINDMILL;
	m2.mTextureId = 0;
	m2.mIsHidden = false;

	mEcs.addComponent<TransformComponent>(e2, std::move(t2));
	mEcs.addComponent<ModelComponent>(e2, std::move(m2));

	initModels();

	WindowManager wm;
	wm.init();

	TestGraphicsPipeline gp(
		wm,
		"src/shaders/GPUDrivenVS.spv",
		"src/shaders/GPUDrivenPS.spv",
		mVertices,
		mIndices,
		mTransforms);

	gp.create();

	// std::thread renderThread(drawLoop, &gp);

	while (wm.isRunning())
	{
		wm.pollEvents();

		mEcs.updateSystem<RenderSystem>(1);

		const auto& renderSystem = mEcs.getSystem<RenderSystem>();
		const auto& instances = renderSystem->getInstances();
		const auto& drawCommands = renderSystem->getDrawCommands();

		gp.updateInstances(instances);
		gp.updateDrawCommands(drawCommands);

		gp.submit(drawCommands);

		if (gp.isResized())
		{
			gp.resize();
		}

		gp.drawFrame(1);
	}

	running = false;
	//renderThread.join();

	gp.destroy();
	wm.destroy();
}

int main() {
	
	testECSandPerMeshRender();
}
