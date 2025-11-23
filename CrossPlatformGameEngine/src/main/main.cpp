//#include "WindowManager/WindowManager.h"
//#include "MyPipelines/Graphics/MyGraphicsPipeline.h"
//#include "MyPipelines/Compute/MyComputePipeline.h"
//#include "Graphics/RenderGraph/RenderGraph.h"
//#include "EcsSystem/EcsSystem.h"
//
//#include "../main/Utility/FrameCounter.h"
//
//#include "Graphics/Engine.h"
//
//#include <atomic>
//#include <thread>
//
//#include <unordered_map>
//
//#include <Mass.h>
//
//#include "MyGame.h"
//
//WindowManager mWindow{};
//EcsSystem mEcs{};
//
//enum MyModels : uint32_t
//{
//	NONE       = 0,
//	PRISM      = 1,
//	SHAPES     = 2,
//	WINDMILL   = 3,
//	HELICOPTER = 4,
//	FROSTY     = 5,
//};
//
//std::unordered_map<uint32_t, ModelData> mModelInfos;
//
//std::vector<float> mVertices;
//std::vector<uint32_t> mIndices;
//std::vector<float> mTransforms;
//
//void initModels()
//{
//	mass::Configuration config{};
//	config.mVertexLayout.mAttributes =
//	{
//		{ 3, sizeof(float), 0 },
//		{ 3, sizeof(float), sizeof(float) * 3 },
//		{ 2, sizeof(float), sizeof(float) * 6 },
//	};
//	config.mHasNormals = true;
//	config.mHasTexCoords = true;
//	config.mHasTransforms = true;
//
//	std::vector<std::pair<uint32_t, std::string>> modelFilepaths =
//	{
//		{ PRISM,      "res/models/prism.model"      },
//		{ SHAPES,     "res/models/shapes.model"     },
//		{ WINDMILL,   "res/models/windmill.model"   },
//		{ HELICOPTER, "res/models/helicopter.model" },
//		{ FROSTY,     "res/models/frosty.model"     },
//	};
//
//	uint32_t globalVertexOffset = 0;
//	uint32_t globalIndexOffset = 0;
//	uint32_t globalTransformOffset = 0;
//
//	for (const auto& path : modelFilepaths)
//	{
//		mass::ModelLayout modelLayout = mass::deserialize(config, path.second);
//
//		uint32_t floatsPerVertex = modelLayout.mVertexLayout.mStride / sizeof(float);
//
//		assert(modelLayout.mVertices.size() % (modelLayout.mVertexLayout.mStride / sizeof(float)) == 0);
//		assert(modelLayout.mTransforms.size() % 16 == 0);
//
//		ModelData info{};
//
//		for (const auto& mesh : modelLayout.mMeshLayouts)
//		{
//			info.mMeshCount++;
//			info.mVertexOffsets.push_back(globalVertexOffset);
//			info.mIndexOffsets.push_back(globalIndexOffset + mesh.mIndexOffset);
//			info.mIndexCounts.push_back(mesh.mIndexCount);
//			info.mTransformOffsets.push_back(globalTransformOffset + mesh.mTransformOffset);
//		}
//
//		mModelInfos.emplace(path.first, std::move(info));
//
//		mVertices.insert(mVertices.end(), modelLayout.mVertices.begin(), modelLayout.mVertices.end());
//		mIndices.insert(mIndices.end(), modelLayout.mIndices.begin(), modelLayout.mIndices.end());
//		mTransforms.insert(mTransforms.end(), modelLayout.mTransforms.begin(), modelLayout.mTransforms.end());
//
//		globalVertexOffset = mVertices.size() / floatsPerVertex;
//		globalIndexOffset = mIndices.size();
//		globalTransformOffset = mTransforms.size() / 16;
//	}
//}
//
//void initEcs()
//{
//	mEcs.registerComponent<TransformComponent>();
//	mEcs.registerComponent<ModelComponent>();
//
//	Signature read = mEcs.getSignature<TransformComponent, ModelComponent>();
//	Signature write{};
//	mEcs.registerSystem<RenderSystem>(read, write, &mModelInfos);
//}
//
//void initEntities()
//{
//	{
//		auto e = mEcs.addEntity();
//
//		TransformComponent t{};
//		t.mPosition = { 0, 0, -10 };
//		t.mRotation = { 0.25, 0, 0 };
//		t.mScale    = { 1, 1, 1 };
//
//		ModelComponent m{};
//		m.mModelId = HELICOPTER;
//		m.mTextureId = 0;
//		m.mIsHidden = false;
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} }); // body
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} }); // main
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} }); // tail
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
//
//		mEcs.addComponent<TransformComponent>(e, std::move(t));
//		mEcs.addComponent<ModelComponent>(e, std::move(m));
//	}
//
//	{
//		auto e = mEcs.addEntity();
//
//		TransformComponent t{};
//		t.mPosition = { 10, 0, 0 };
//		t.mRotation = { 0, 0, 0.78f };
//		t.mScale = { 1, 1, 1 };
//
//		ModelComponent m{};
//		m.mModelId = SHAPES;
//		m.mTextureId = 1;
//		m.mIsHidden = false;
//		m.mMeshTransforms.push_back({ {-2, 0, 0}, {0, 0, 0}, {2, 1, 1} });
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 2, 1} });
//		m.mMeshTransforms.push_back({ {2, 0, 0}, {0, 0, 0}, {1, 1, 2} });
//
//		mEcs.addComponent<TransformComponent>(e, std::move(t));
//		mEcs.addComponent<ModelComponent>(e, std::move(m));
//	}
//
//	{
//		auto e = mEcs.addEntity();
//
//		TransformComponent t{};
//		t.mPosition = { -10, -10, -5 };
//		t.mRotation = { 0, 0, 0 };
//		t.mScale = { 1, 1, 1 };
//
//		ModelComponent m{};
//		m.mModelId = WINDMILL;
//		m.mTextureId = 2;
//		m.mIsHidden = false;
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
//
//		mEcs.addComponent<TransformComponent>(e, std::move(t));
//		mEcs.addComponent<ModelComponent>(e, std::move(m));
//	}
//
//	{
//		auto e = mEcs.addEntity();
//
//		TransformComponent t{};
//		t.mPosition = { 0, -4, 0 };
//		t.mRotation = { 0, 0, 0 };
//		t.mScale = { 1, 1, 1 };
//
//		ModelComponent m{};
//		m.mModelId = FROSTY;
//		m.mTextureId = 0;
//		m.mIsHidden = false;
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
//		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
//
//		mEcs.addComponent<TransformComponent>(e, std::move(t));
//		mEcs.addComponent<ModelComponent>(e, std::move(m));
//	}
//}
//
//void initResources()
//{
//
//}
//
//ascen::RenderGraph mRenderGraph;
//
//void initRenderGraph()
//{
//	ascen::RenderGraphNode n0;
//	n0.mResourceType = ascen::ResourceType::BUFFER;
//	n0.mAccess = ascen::Access::READ;
//	n0.mStage = ascen::Stage::VERTEX;
//	n0.mPipeline = 0;
//	// n0.mResource = 0;
//}
//
//int main()
//{
//	mWindow.create();
//	/*
//
//	initModels();
//	initEcs();
//	initEntities();
//
//	MyGraphicsPipeline graphics(
//		&mWindow,
//		"src/shaders/GPUDrivenVS.spv",
//		"src/shaders/GPUDrivenPS.spv",
//		mVertices,
//		mIndices,
//		mTransforms);
//	graphics.create();
//
//	MyComputePipeline compute(
//		"src/shaders/GPUDrivenCS.spv",
//		0);
//
//	FrameCounter frameCounter(120);
//
//	glm::vec3 camUpWorld(0.0f, 1.0f, 0.0f);
//	glm::vec3 camPosition(0.0f);
//	glm::vec3 camRotation(0.0f); // radians: x=pitch, y=yaw
//	float camSpeed = 10.0f;
//
//	float timePassed = 0;
//
//	while (mWindow.isRunning())
//	{
//		frameCounter.frame();
//
//		const float delta = frameCounter.deltaTime();
//
//		timePassed += delta;
//		if (timePassed > 1.0f)
//		{
//			std::cout << frameCounter.fps() << "\n";
//			timePassed = 0.0f;
//		}
//
//		if (mWindow.getInput().isKeyPressed(GLFW_KEY_ESCAPE))
//			break;
//
//		if (graphics.isResized())
//			graphics.resize();
//
//		// --- yaw (Q/E), pitch clamped ---
//		if (mWindow.getInput().isKeyPressed(GLFW_KEY_Q)) camRotation.y -= camSpeed * 0.25f * delta; // look left
//		if (mWindow.getInput().isKeyPressed(GLFW_KEY_E)) camRotation.y += camSpeed * 0.25f * delta; // look right
//		camRotation.x = glm::clamp(camRotation.x, -1.553f, 1.553f); // ±89°
//
//		// --- derive camera basis (-Z forward, Y up) ---
//		glm::vec3 camForward;
//		camForward.x = std::cos(camRotation.x) * std::sin(camRotation.y);
//		camForward.y = std::sin(camRotation.x);
//		camForward.z = -std::cos(camRotation.x) * std::cos(camRotation.y);
//		camForward = glm::normalize(camForward);
//
//		glm::vec3 camRight = glm::normalize(glm::cross(camForward, glm::vec3(0, 1, 0)));
//		glm::vec3 camUp = glm::normalize(glm::cross(camRight, camForward));
//
//		// --- movement ---
//		if (mWindow.getInput().isKeyPressed(GLFW_KEY_A)) camPosition -= camRight * camSpeed * delta;
//		if (mWindow.getInput().isKeyPressed(GLFW_KEY_D)) camPosition += camRight * camSpeed * delta;
//		if (mWindow.getInput().isKeyPressed(GLFW_KEY_W)) camPosition += camForward * camSpeed * delta;
//		if (mWindow.getInput().isKeyPressed(GLFW_KEY_S)) camPosition -= camForward * camSpeed * delta;
//
//		// --- build camera transform ---
//		glm::mat4 rot(1.0f);
//		rot[0] = glm::vec4(camRight, 0.0f);
//		rot[1] = glm::vec4(camUp, 0.0f);
//		rot[2] = glm::vec4(-camForward, 0.0f); // note the negative
//
//		glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), camPosition) * rot;
//
//		// view = inverse(cameraTransform)
//		graphics.updateCamera(cameraTransform);
//
//		// update entities
//
//		{ // helicopter
//			auto& t = mEcs.getComponent<TransformComponent>(0);
//			t.mRotation += glm::vec3(0, 1.0f, 0) * delta;
//
//			auto& m = mEcs.getComponent<ModelComponent>(0);
//			m.mMeshTransforms[1].mRotation += glm::vec3(0, 10.0f, 0) * delta; // main
//			m.mMeshTransforms[2].mRotation += glm::vec3(40.0f, 0, 0) * delta; // tail
//		}
//
//		{ // windmill
//			auto& m = mEcs.getComponent<ModelComponent>(2);
//			m.mMeshTransforms[1].mRotation += glm::vec3(0, 5.0f, 0) * delta; // fan
//		}
//
//		mEcs.updateSystem<RenderSystem>(delta);
//
//		const auto& renderSystem = mEcs.getSystem<RenderSystem>();
//		const auto& instances = renderSystem->getInstances();
//		const auto& drawCommands = renderSystem->getDrawCommands();
//
//		graphics.updateInstances(instances);
//		graphics.updateDrawCommands(drawCommands);
//
//		graphics.submit(drawCommands);
//
//		graphics.drawFrame();
//	}
//
//	graphics.destroy();
//	mWindow.destroy();
//
//
//	return 0;
//	*/
//
//	MyGame game(&mWindow);
//
//	game.destroy();
//	mWindow.destroy();
//
//	return 0;
//}

#include "WindowManager/WindowManager.h"
#include "MyGame2.h"

#include "Utility/FrameCounter.h"

WindowManager mWindow;
MyGame2* mGame = nullptr;

int main()
{
	FrameCounter frameCounter(1000);
	float timePassed = 0;

	mWindow.create();

	mGame = new MyGame2(mWindow);

	while (mWindow.isRunning())
	{
		frameCounter.frame();
		const float delta = frameCounter.deltaTime();

		timePassed += delta;
		if (timePassed > 1.0f)
		{
			std::cout << frameCounter.fps() << "\n";
			timePassed = 0.0f;
		}

		if (mWindow.getInput().isKeyPressed(GLFW_KEY_ESCAPE))
		{
			break;
		}

		mGame->run(delta);
	}

	mGame->cleanup();
	mWindow.destroy();

	delete mGame;

	return 0;
}
