#include "../main/WindowManager/WindowManager.h"
#include "../main/MyGraphicsPipeline/Test/TestGraphicsPipeline.h"
#include "../main/EcsSystem/EcsSystem.h"

#include "../main/Utility/FrameCounter.h"

#include <atomic>
#include <thread>

#include <unordered_map>

#include <Mass.h>

WindowManager mWindow{};
EcsSystem mEcs{};

enum MyModels : uint32_t
{
	NONE       = 0,
	PRISM      = 1,
	SHAPES     = 2,
	WINDMILL   = 3,
	HELICOPTER = 4,
	FROSTY     = 5,
};

std::unordered_map<uint32_t, ModelData> mModelInfos;

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
		{ PRISM,      "res/models/prism.model"      },
		{ SHAPES,     "res/models/shapes.model"     },
		{ WINDMILL,   "res/models/windmill.model"   },
		{ HELICOPTER, "res/models/helicopter.model" },
		{ FROSTY,     "res/models/frosty.model"     },
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

void initEcs()
{
	mEcs.registerComponent<TransformComponent>();
	mEcs.registerComponent<ModelComponent>();

	Signature read = mEcs.getSignature<TransformComponent, ModelComponent>();
	Signature write{};
	mEcs.registerSystem<RenderSystem>(read, write, &mModelInfos);
}

void initEntities()
{
	{
		auto e = mEcs.addEntity();

		TransformComponent t{};
		t.mPosition = { 0, 0, -5 };
		t.mRotation = { 0.25, 0, 0 };
		t.mScale    = { 1, 1, 1 };

		ModelComponent m{};
		m.mModelId = HELICOPTER;
		m.mTextureId = 0;
		m.mIsHidden = false;
		m.mMeshTransforms.push_back({ {0, 0, 0},    {0, 0, 0}, {1, 1, 1} }); // body
		m.mMeshTransforms.push_back({ {0, 1.75, -0.2}, {0, 0, 0}, {1, 1, 1} }); // main
		m.mMeshTransforms.push_back({ {0, 1.5, -7},  {0, 0, 0}, {1, 1, 1} }); // tail
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });

		mEcs.addComponent<TransformComponent>(e, std::move(t));
		mEcs.addComponent<ModelComponent>(e, std::move(m));
	}

	{
		auto e = mEcs.addEntity();

		TransformComponent t{};
		t.mPosition = { 10, 0, 0 };
		t.mRotation = { 0, 0, 0.78f };
		t.mScale = { 1, 1, 1 };

		ModelComponent m{};
		m.mModelId = SHAPES;
		m.mTextureId = 1;
		m.mIsHidden = false;
		m.mMeshTransforms.push_back({ {-2, 0, 0}, {0, 0, 0}, {2, 1, 1} });
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 2, 1} });
		m.mMeshTransforms.push_back({ {2, 0, 0}, {0, 0, 0}, {1, 1, 2} });

		mEcs.addComponent<TransformComponent>(e, std::move(t));
		mEcs.addComponent<ModelComponent>(e, std::move(m));
	}

	{
		auto e = mEcs.addEntity();

		TransformComponent t{};
		t.mPosition = { 0, 10, 0 };
		t.mRotation = { 0, 0, 0 };
		t.mScale = { 1, 1, 1 };

		ModelComponent m{};
		m.mModelId = WINDMILL;
		m.mTextureId = 2;
		m.mIsHidden = false;
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
		m.mMeshTransforms.push_back({ {0, 0, 8.5f}, {0, 0, 0}, {1, 1, 1} });

		mEcs.addComponent<TransformComponent>(e, std::move(t));
		mEcs.addComponent<ModelComponent>(e, std::move(m));
	}

	{
		auto e = mEcs.addEntity();

		TransformComponent t{};
		t.mPosition = { 0, -4, 0 };
		t.mRotation = { 0, 0, 0 };
		t.mScale = { 1, 1, 1 };

		ModelComponent m{};
		m.mModelId = FROSTY;
		m.mTextureId = 0;
		m.mIsHidden = false;
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });

		mEcs.addComponent<TransformComponent>(e, std::move(t));
		mEcs.addComponent<ModelComponent>(e, std::move(m));
	}
}

int main()
{
	mWindow.create();

	initModels();
	initEcs();
	initEntities();

	TestGraphicsPipeline pipeline(
		&mWindow,
		"src/shaders/GPUDrivenVS.spv",
		"src/shaders/GPUDrivenPS.spv",
		mVertices,
		mIndices,
		mTransforms);
	pipeline.create();

	FrameCounter frameCounter;

	glm::vec3 camUpWorld(0.0f, 1.0f, 0.0f);
	glm::vec3 camPosition(0.0f);
	glm::vec3 camRotation(0.0f); // radians: x=pitch, y=yaw
	float camSpeed = 10.0f;

	while (mWindow.isRunning())
	{
		frameCounter.frame();
		const float delta = frameCounter.deltaTime();

		if (mWindow.getInput().isKeyPressed(GLFW_KEY_ESCAPE))
			break;

		if (pipeline.isResized())
			pipeline.resize();

		// --- yaw (Q/E), pitch clamped ---
		if (mWindow.getInput().isKeyPressed(GLFW_KEY_Q)) camRotation.y -= camSpeed * 0.5f * delta; // look left
		if (mWindow.getInput().isKeyPressed(GLFW_KEY_E)) camRotation.y += camSpeed * 0.5f * delta; // look right
		camRotation.x = glm::clamp(camRotation.x, -1.553f, 1.553f); // ±89°

		// --- derive camera basis (-Z forward, Y up) ---
		glm::vec3 camForward;
		camForward.x = std::cos(camRotation.x) * std::sin(camRotation.y);
		camForward.y = std::sin(camRotation.x);
		camForward.z = -std::cos(camRotation.x) * std::cos(camRotation.y);
		camForward = glm::normalize(camForward);

		glm::vec3 camRight = glm::normalize(glm::cross(camForward, glm::vec3(0, 1, 0)));
		glm::vec3 camUp = glm::normalize(glm::cross(camRight, camForward));

		// --- movement ---
		if (mWindow.getInput().isKeyPressed(GLFW_KEY_A)) camPosition -= camRight * camSpeed * delta;
		if (mWindow.getInput().isKeyPressed(GLFW_KEY_D)) camPosition += camRight * camSpeed * delta;
		if (mWindow.getInput().isKeyPressed(GLFW_KEY_W)) camPosition += camForward * camSpeed * delta;
		if (mWindow.getInput().isKeyPressed(GLFW_KEY_S)) camPosition -= camForward * camSpeed * delta;

		// --- build camera transform ---
		glm::mat4 rot(1.0f);
		rot[0] = glm::vec4(camRight, 0.0f);
		rot[1] = glm::vec4(camUp, 0.0f);
		rot[2] = glm::vec4(-camForward, 0.0f); // note the negative

		glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), camPosition) * rot;

		// view = inverse(cameraTransform)
		pipeline.updateCamera(cameraTransform);


		// update entities
		auto& t = mEcs.getComponent<TransformComponent>(0);
		t.mRotation += glm::vec3(0, 2.0f, 0) * delta;

		auto& m = mEcs.getComponent<ModelComponent>(0);
		m.mMeshTransforms[1].mRotation += glm::vec3(0, 10.0f, 0) * delta; // main
		m.mMeshTransforms[2].mRotation += glm::vec3(40.0f, 0, 0) * delta; // tail

		mEcs.updateSystem<RenderSystem>(delta);

		const auto& renderSystem = mEcs.getSystem<RenderSystem>();
		const auto& instances = renderSystem->getInstances();
		const auto& drawCommands = renderSystem->getDrawCommands();

		pipeline.updateInstances(instances);
		pipeline.updateDrawCommands(drawCommands);

		pipeline.submit(drawCommands);

		pipeline.drawFrame();
	}

	pipeline.destroy();
	mWindow.destroy();


	return 0;
}
