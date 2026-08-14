#include "MyGame.h"

#include <cmath>

#include "../WindowManager/WindowManager.h"
#include "../Utility/ImageLoader/ImageLoader.h"

#include "Ecs/Components/ModelComponent.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Components/PhysicsBodyComponent.h"

#include "Ecs/Systems/FrustumCullingSystem.h"
#include "Ecs/Systems/PhysicsSystem.h"

MyGame::MyGame(WindowManager& windowManager) :
	mWindowManager(windowManager),
	mEngine(windowManager),
	mPipeline(mEngine)
{
	mPipeline.init();

	mEngine.reload();

	mModelHandler.loadModels({ "assets/models/test.model", "assets/models/submarine.model", "assets/models/sphere.model" });

	const auto& models = mModelHandler.getModels();
	const auto& vertices = mModelHandler.getVertices();
	const auto& indices = mModelHandler.getIndices();

	mEngine.uploadBuffer("BUFFER_VERTEX", &vertices[0], vertices.size(), sizeof(float), 0);
	mEngine.uploadBuffer("BUFFER_INDEX", &indices[0], indices.size(), sizeof(uint32_t), 0);

	// initialize ECS
	mEngine.ecs().registerComponent<TransformComponent>();
	mEngine.ecs().registerComponent<ModelComponent>();
	mEngine.ecs().registerComponent<PhysicsBodyComponent>();

	{
		const auto readSig = mEngine.ecs().getSignature<PhysicsBodyComponent>();
		const auto writeSig = mEngine.ecs().getSignature<TransformComponent>();

		mEngine.ecs().registerSystem<PhysicsSystem>(readSig, writeSig);
	}

	{
		const auto readSig = mEngine.ecs().getSignature<TransformComponent, ModelComponent>();
		const auto writeSig = mEngine.ecs().getSignature<>();

		mEngine.ecs().registerSystem<FrustumCullingSystem>(readSig, writeSig, mEngine, models);
	}

	loadTextures();

	mEngine.uploadTexture("TEXTURE", mPixels);

	constexpr double r = 50;
	constexpr double deg = 360;

	// entities
	for (double i = 0; i < deg; i += (deg / mEntityCount))
	{
		const double angle = i * (M_PI / 180);
		double x = r * std::cos(angle);
		double z = r * std::sin(angle);

		if (i < 180)
		{
			createModel("assets/models/test.model", { x, 15, z }, { 1, 1, 1 });
		}
		else
		{
			createModel("assets/models/submarine.model", { x, 15, z }, { 1, 1, 1 });
		}
	}

	createPhysicsWorld();
}

void MyGame::run(float delta)
{
	updateCamera(delta);

	for (unsigned int i = 0; i < static_cast<unsigned int>(mEntityCount); i++)
	{
		if (i % 5 == 0)
		{
			auto& transform = mEngine.ecs().getComponent<TransformComponent>(i);

			transform.mRotation.x += 2.0f * delta;
			transform.mRotation.y += 2.0f * delta;
			transform.mRotation.z += 2.0f * delta;

			transform.mScale.x = 2.0f;
			transform.mScale.y = 2.0f;
			transform.mScale.z = 2.0f;
		}
	}

	updateMousePicking();

	uint32_t selectedEntity = 0;
	mEngine.downloadTransfer<uint32_t>("FRAMEPASS_MOUSE_PICKING_TRANSFER", 0, &selectedEntity);
	selectedEntity--;

	if (selectedEntity < ENTITY_MAX)
	{
		auto& current = mEngine.ecs().getComponent<ModelComponent>(selectedEntity);
		current.mIsSelected = 1;
	}

	mEngine.ecs().updateSystem<PhysicsSystem>(delta);
	mEngine.ecs().updateSystem<FrustumCullingSystem>(delta);

	mEngine.drawFrame();

	if (selectedEntity < ENTITY_MAX)
	{
		auto& current = mEngine.ecs().getComponent<ModelComponent>(selectedEntity);
		current.mIsSelected = 0;
	}
}

void MyGame::cleanup()
{
	mEngine.cleanup();
}

void MyGame::loadTextures()
{
	const std::vector<const char*> mTextureFilepaths =
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
		ImageLoader::loadImage(filepath, &raw);
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

	PhysicsBodyComponent p{};
	p.mBodyType = DYNAMIC;
	p.mDensity = 3.0f;

	mEngine.ecs().addComponent<TransformComponent>(e, std::move(t));
	mEngine.ecs().addComponent<ModelComponent>(e, std::move(m));
	mEngine.ecs().addComponent<PhysicsBodyComponent>(e, std::move(p));
}

void MyGame::createPhysicsWorld()
{
	mWorldDef = b3DefaultWorldDef();
	mWorldId  = b3CreateWorld(&mWorldDef);

	b3BodyDef bodyDef = b3DefaultBodyDef();
	bodyDef.type = b3_dynamicBody;
	bodyDef.position = { -3.0f, 8.0f };
	bodyDef.name = "crate1";
	b3BodyId bodyId = b3CreateBody(mWorldId, &bodyDef);

	b3BoxHull box = b3MakeBoxHull(0.75f, 0.75f, 0.75f);

	b3ShapeDef shapeDef = b3DefaultShapeDef();
	shapeDef.density = 2.0f;

	b3CreateHullShape(bodyId, &shapeDef, &box.base);
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
		mEngine.updatePushConstant<glm::mat4>("PIPELINE_FRUSTUM_CULL",  0, vp);
		mEngine.updatePushConstant<glm::mat4>("PIPELINE_RENDER_ENTITY", 0, vp);
		mEngine.updatePushConstant<glm::mat4>("PIPELINE_RENDER_BOUNDS", 0, vp);
		mEngine.updatePushConstant<glm::mat4>("PIPELINE_MOUSE_PICKING", 0, vp);
	}
}

void MyGame::updateMousePicking()
{
	const double mouseX = mWindowManager.getInput().getMouseX();
	const double mouseY = mWindowManager.getInput().getMouseY();

	const int windowWidth = WindowManager::getWidth();
	const int windowHeight = WindowManager::getHeight();

	const int pixelX = std::clamp(
		static_cast<int>(mouseX),
		0,
		windowWidth - 1);

	const int pixelY = std::clamp(
		static_cast<int>(mouseY),
		0,
		windowHeight - 1);

	ascen::transfer::BufferImageRegion region
	{
		.mBufferOffset = 0,
		.mImageOffset = { pixelX, pixelY, 0 },
		.mImageExtent = { 1, 1, 1 },
	};

	if (pixelX >= 0 && pixelY >= 0 && pixelX < windowWidth && pixelY < windowHeight)
	{
		mEngine.updateTransfer("FRAMEPASS_MOUSE_PICKING_TRANSFER", 0, region);
	}
}
