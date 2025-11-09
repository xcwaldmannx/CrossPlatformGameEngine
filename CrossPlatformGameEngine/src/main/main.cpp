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

#include "Utility/FrameCounter.h"

#include <thread>

#include <string>

#include <Mass.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

auto startTime = std::chrono::high_resolution_clock::now();
auto endTime = std::chrono::high_resolution_clock::now();

std::atomic<bool> running = true;

void drawLoop(TestGraphicsPipeline* gp, EcsSystem* ecs)
{
	FrameCounter frameCounter;

	while (running)
	{
		frameCounter.frame();

		auto& e0t = ecs->getComponent<TransformComponent>(0);
		e0t.mRotation += glm::vec3(0, 0, 0.001f);

		auto& e0m0m0 = ecs->getComponent<ModelComponent>(0);
		e0m0m0.mMeshTransforms[0].mRotation += glm::vec3(0, 0, 0.025f);
		auto& e0m0m2 = ecs->getComponent<ModelComponent>(0);
		e0m0m2.mMeshTransforms[2].mRotation += glm::vec3(0.05f, 0, 0);



		auto& e2t = ecs->getComponent<TransformComponent>(2);
		e2t.mRotation += glm::vec3(0.001f, 0.001f, 0.001f);

		auto& e2m2m1 = ecs->getComponent<ModelComponent>(2);
		e2m2m1.mMeshTransforms[1].mRotation += glm::vec3(0, 0.01f, 0);

		ecs->updateSystem<RenderSystem>(frameCounter.deltaTime());

		const auto& renderSystem = ecs->getSystem<RenderSystem>();
		const auto& instances = renderSystem->getInstances();
		const auto& drawCommands = renderSystem->getDrawCommands();

		gp->updateInstances(instances);
		gp->updateDrawCommands(drawCommands);

		gp->submit(drawCommands);

		if (gp->isResized())
		{
			gp->resize();
		}

		gp->drawFrame(0);

		std::cout << frameCounter.fps() << "\n";
	}
}

#include "Graphics/Ecs/Components/ModelComponent.h"
#include "Graphics/Ecs/Components/TransformComponent.h"

#include "Graphics/Ecs/Systems/RenderSystem.h"

EcsSystem mEcs;

std::unordered_map<uint32_t, ModelData> mModelInfos;

void initECS(EcsSystem* ecs)
{
	ecs->registerComponent<TransformComponent>();
	ecs->registerComponent<ModelComponent>();

	Signature readSig = ecs->getSignature<TransformComponent, ModelComponent>();
	Signature writeSig = ecs->getSignature<ModelComponent>();
	ecs->registerSystem<RenderSystem>(readSig, writeSig, &mModelInfos);
}

enum MyModels : uint32_t
{
	NONE       = 0,
	PRISM      = 1,
	SHAPES     = 2,
	WINDMILL   = 3,
	HELICOPTER = 4,
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
		{ HELICOPTER, "res/models/helicopter.model" },
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
	initECS(&mEcs);



	auto e0 = mEcs.addEntity();

	TransformComponent t0{};
	t0.mPosition = { 0, 0, -3 };
	t0.mRotation = {  0, 0.25f, -1.6 };
	t0.mScale    = {  2, 2, 2 };

	ModelComponent m0{};
	m0.mModelId = HELICOPTER;
	m0.mTextureId = 0;
	m0.mIsHidden = false;
	m0.mMeshTransforms.push_back({ {0, 0, 1.75}, {0, 0, 0}, {1, 1, 1} }); // main
	m0.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} }); // body
	m0.mMeshTransforms.push_back({ {0, 7, 1.5}, {0, 0, 0}, {1, 1, 1} }); // tail

	mEcs.addComponent<TransformComponent>(e0, std::move(t0));
	mEcs.addComponent<ModelComponent>(e0, std::move(m0));



	auto e1 = mEcs.addEntity();

	TransformComponent t1{};
	t1.mPosition = { 10, 0, 0 };
	t1.mRotation = { 0, 0, 0.78f };
	t1.mScale    = { 1, 1, 1 };

	ModelComponent m1{};
	m1.mModelId = SHAPES;
	m1.mTextureId = 0;
	m1.mIsHidden = false;
	m1.mMeshTransforms.push_back({ {-2, 0, 0}, {0, 0, 0}, {2, 1, 1} });
	m1.mMeshTransforms.push_back({  {0, 0, 0}, {0, 0, 0}, {1, 2, 1} });
	m1.mMeshTransforms.push_back({  {2, 0, 0}, {0, 0, 0}, {1, 1, 2} });

	mEcs.addComponent<TransformComponent>(e1, std::move(t1));
	mEcs.addComponent<ModelComponent>(e1, std::move(m1));

	auto e2 = mEcs.addEntity();

	TransformComponent t2{};
	t2.mPosition = { 0, 10, 0 };
	t2.mRotation = { 0, 0, 0 };
	t2.mScale    = { 1, 1, 1 };

	ModelComponent m2{};
	m2.mModelId = WINDMILL;
	m2.mTextureId = 0;
	m2.mIsHidden = false;
	m2.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
	m2.mMeshTransforms.push_back({ {0, 0, 8.5f}, {0, 0, 0}, {1, 1, 1} });

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

	std::thread renderThread(drawLoop, &gp, &mEcs);

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
	
	testECSandPerMeshRender();
}
