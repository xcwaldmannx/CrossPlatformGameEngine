#include "MyGame.h"

#include <cmath>

#include "../WindowManager/WindowManager.h"
#include "../Utility/ImageLoader/ImageLoader.h"

#include "Ecs/Components/ModelComponent.h"
#include "Ecs/Components/TransformComponent.h"

#include "Ecs/Systems/FrustumCullingSystem.h"
#include "Ecs/Systems/PhysicsSystem.h"

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

	{
		const auto readSig = mEngine.ecs().getSignature<TransformComponent, ModelComponent>();
		const auto writeSig = mEngine.ecs().getSignature<>();

		mEngine.ecs().registerSystem<PhysicsSystem>(readSig, writeSig);
	}

	{
		const auto readSig = mEngine.ecs().getSignature<>();
		const auto writeSig = mEngine.ecs().getSignature<TransformComponent>();

		mEngine.ecs().registerSystem<FrustumCullingSystem>(readSig, writeSig, mEngine, models);
	}

	loadTextures();

	mEngine.resource().uploadTexture("TEXTURE", mPixels);

	double r = 40;
	double deg = 360;
	unsigned int entityCount = 0;

	for (double i = 0; i < deg; i += (deg / mEntityCount))
	{
		const double angle = i * (M_PI / 180);
		double x = r * std::cos(angle);
		double z = r * std::sin(angle);

		if (i > 180)
		{
			createModel("assets/models/test.model", { x, 5, z }, { 1, 1, 1 });
		}
		else
		{
			createModel("assets/models/submarine.model", { x, 5, z }, { 1, 1, 1 });
		}

		entityCount++;
	}
}

void MyGame::run(float delta)
{
	updateCamera(delta);
	mEngine.ecs().updateSystem<FrustumCullingSystem>(delta);
	mEngine.ecs().updateSystem<PhysicsSystem>(delta);

	for (unsigned int i = 0; i < static_cast<unsigned int>(mEntityCount); i++)
	{
		if (i % 5 == 0)
		{
			auto& transform = mEngine.ecs().getComponent<TransformComponent>(i);
			transform.mRotation.x += 4 * delta;
			transform.mRotation.z += 4 * delta;
		}
	}

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

void MyGame::createModel(const std::string& model, const glm::vec3 position, const glm::vec3 scale)
{
	const auto e = mEngine.ecs().addEntity();

	TransformComponent t{};
	t.mPosition = position;
	t.mRotation = { 0, 0, 0 };
	t.mScale = scale;

	ModelComponent m{};
	m.mName = model;
	m.mModelId = 0;
	m.mTextureId = 0;
	m.mIsHidden = false;

	mEngine.ecs().addComponent<TransformComponent>(e, std::move(t));
	mEngine.ecs().addComponent<ModelComponent>(e, std::move(m));
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

		const glm::mat4 vp = ubo.mProj * ubo.mView;
		mEngine.pipeline().pushConstants("PIPELINE_FRUSTUM_CULL",  "PUSH_0", &vp);
		mEngine.pipeline().pushConstants("PIPELINE_RENDER_ENTITY", "PUSH_0", &vp);
	}
}
