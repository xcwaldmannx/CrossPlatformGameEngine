#include "MyGame.h"

#include "../WindowManager/WindowManager.h"
#include "../Utility/ImageLoader/ImageLoader.h"

#include "Ecs/Components/ModelComponent.h"
#include "Ecs/Components/TransformComponent.h"

#include "Ecs/Systems/FrustumCullingSystem.h"
#include "Ecs/Systems/SimpleRenderSystem.h"

MyGame::MyGame(WindowManager& windowManager) :
	mWindowManager(windowManager),
	mEngine(windowManager),
	mPipeline(mEngine)
{
	mPipeline.init();

	mEngine.reload();

	mModelHandler.loadModels({ "assets/models/test.model", "assets/models/submarine.model" });

	const auto& models = mModelHandler.getModels();
	const auto& vertices = mModelHandler.getVertices();
	const auto& indices = mModelHandler.getIndices();

	mEngine.resource().uploadBuffer("BUFFER_VERTEX", &vertices[0], vertices.size(), sizeof(float));
	mEngine.resource().uploadBuffer("BUFFER_INDEX", &indices[0], indices.size(), sizeof(uint32_t));

	// initialize ECS
	mEngine.ecs().registerComponent<TransformComponent>();
	mEngine.ecs().registerComponent<ModelComponent>();

	const auto readSig = mEngine.ecs().getSignature<TransformComponent, ModelComponent>();
	const auto writeSig = mEngine.ecs().getSignature<>();

	mEngine.ecs().registerSystem<FrustumCullingSystem>(readSig, writeSig, mEngine, models);

	loadTextures();

	mEngine.resource().updateTexture("TEXTURE", mPixels);

	float r = 50.0f;

	for (float i = 0; i < 1440.0f; i += (1440.0f / 256.0f))
	{
		float angle = i * (M_PI / 180.0f);
		float x = (r - (i / r) * 0.5f) * cos(angle);
		float z = (r - (i / r) * 0.5f) * sin(angle);

		createModel({ x, i * 0.01f, z });
	}
}

void MyGame::run(float delta)
{
	updateCamera(delta);
	mEngine.ecs().updateSystem<FrustumCullingSystem>(delta);
	mEngine.drawFrame();
}

void MyGame::cleanup()
{
	mEngine.cleanup();
}

void MyGame::loadTextures()
{
	ImageLoader il;

	std::vector<const char*> mTextureFilepaths =
	{
		"assets/textures/metal.jpg",
	};

	uint32_t width = 1024;
	uint32_t height = 1024;
	uint32_t channels = 4;

	mPixels.reserve(width * height * channels * mTextureFilepaths.size());

	for (const auto& filepath : mTextureFilepaths)
	{
		RawImage raw;
		il.loadImage(filepath, &raw);
		mPixels.insert(mPixels.end(), raw.mPixels.begin(), raw.mPixels.end());
	}
}

void MyGame::createEntities()
{
	{
		auto e = mEngine.ecs().addEntity();

		TransformComponent t{};
		t.mPosition = { 0, 0, -10 };
		t.mRotation = { 0.25, 0, 0 };
		t.mScale = { 1, 1, 1 };

		ModelComponent m{};
		m.mModelId = HELICOPTER;
		m.mTextureId = 0;
		m.mIsHidden = false;
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} }); // body
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} }); // main
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} }); // tail
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
		m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });

		mEngine.ecs().addComponent<TransformComponent>(e, std::move(t));
		mEngine.ecs().addComponent<ModelComponent>(e, std::move(m));
	}
}

void MyGame::createHelicopter(glm::vec3 position)
{
	auto e = mEngine.ecs().addEntity();

	TransformComponent t{};
	t.mPosition = position;
	t.mRotation = { 0, 0, 0 };
	t.mScale = { 1, 1, 1 };

	ModelComponent m{};
	m.mModelId = HELICOPTER;
	m.mTextureId = 0;
	m.mIsHidden = false;
	m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} }); // body
	m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} }); // main
	m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} }); // tail
	m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });
	m.mMeshTransforms.push_back({ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} });

	mEngine.ecs().addComponent<TransformComponent>(e, std::move(t));
	mEngine.ecs().addComponent<ModelComponent>(e, std::move(m));
}

void MyGame::createModel(const glm::vec3 position)
{
	const auto e = mEngine.ecs().addEntity();

	TransformComponent t{};
	t.mPosition = position;
	t.mRotation = { 0, 0, 0 };
	t.mScale = { 1, 1, 1 };

	ModelComponent m{};
	m.mName = "assets/models/test.model";
	m.mModelId = 0;
	m.mTextureId = 0;
	m.mIsHidden = false;

	mEngine.ecs().addComponent<TransformComponent>(e, std::move(t));
	mEngine.ecs().addComponent<ModelComponent>(e, std::move(m));
}

void MyGame::updateEntities(float delta)
{
	for (int i = 0; i < 2000; i++)
	{ // helicopters
		auto& t = mEngine.ecs().getComponent<TransformComponent>(i);
		t.mRotation += glm::vec3(0, 1.0f, 0) * delta;

		auto& m = mEngine.ecs().getComponent<ModelComponent>(i);
		//m.mMeshTransforms[1].mRotation += glm::vec3(0, 10.0f, 0) * delta; // main
		m.mMeshTransforms[2].mRotation += glm::vec3(0, 10, 0) * delta; // tail
	}
}

void MyGame::updateCamera(float delta)
{
	// --- yaw (Q/E), pitch clamped ---
	if (mWindowManager.getInput().isKeyPressed(GLFW_KEY_Q)) camRotation.y -= camSpeed * 0.25f * delta; // look left
	if (mWindowManager.getInput().isKeyPressed(GLFW_KEY_E)) camRotation.y += camSpeed * 0.25f * delta; // look right
	camRotation.x = glm::clamp(camRotation.x, -1.553f, 1.553f);
	
	// --- derive camera basis (-Z forward, Y up) ---
	glm::vec3 camForward;
	camForward.x = std::cos(camRotation.x) * std::sin(camRotation.y);
	camForward.y = std::sin(camRotation.x);
	camForward.z = -std::cos(camRotation.x) * std::cos(camRotation.y);
	camForward = glm::normalize(camForward);
	
	glm::vec3 camRight = glm::normalize(glm::cross(camForward, glm::vec3(0, 1, 0)));
	glm::vec3 camUp = glm::normalize(glm::cross(camRight, camForward));
	
	// --- movement ---
	if (mWindowManager.getInput().isKeyPressed(GLFW_KEY_A)) camPosition -= camRight * camSpeed * delta;
	if (mWindowManager.getInput().isKeyPressed(GLFW_KEY_D)) camPosition += camRight * camSpeed * delta;
	if (mWindowManager.getInput().isKeyPressed(GLFW_KEY_W)) camPosition += camForward * camSpeed * delta;
	if (mWindowManager.getInput().isKeyPressed(GLFW_KEY_S)) camPosition -= camForward * camSpeed * delta;
	
	// --- build camera transform ---
	glm::mat4 rot(1.0f);
	rot[0] = glm::vec4(camRight, 0.0f);
	rot[1] = glm::vec4(camUp, 0.0f);
	rot[2] = glm::vec4(-camForward, 0.0f); // note the negative
	
	glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), camPosition) * rot;
	
	const auto width = static_cast<float>(mEngine.getScreenWidth());
	const auto height = static_cast<float>(mEngine.getScreenHeight());

	if (width * height > 0)
	{
		Camera ubo{};
		ubo.mView = glm::inverse(cameraTransform);
		ubo.mProj = glm::perspective(glm::radians(90.0f),
			width / height, 0.01f, 100'000.0f);
		ubo.mProj[1][1] *= -1;

		mEngine.resource().uploadBuffer("BUFFER_CAMERA", &ubo, 1, sizeof(Camera), mEngine.getFrameIndex());
	}
}
