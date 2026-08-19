#include "MyGame.h"

#include <cmath>

#include "../WindowManager/WindowManager.h"
#include "../Utility/ImageLoader/ImageLoader.h"

#include "Ecs/Components/ModelComponent.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Components/PhysicsBodyComponent.h"
#include "Ecs/Components/MoverComponent.h"

#include "Ecs/Systems/FrustumCullingSystem.h"
#include "Ecs/Systems/PhysicsSystem.h"
#include "Ecs/Systems/MoverSystem.h"

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

	// init physics world
	b3WorldDef worldDef = b3DefaultWorldDef();
	worldDef.gravity = { 0.0f, -9.81f, 0.0f };
	mWorldId  = b3CreateWorld(&worldDef);

	// initialize ECS
	mEngine.ecs().registerComponent<TransformComponent>();
	mEngine.ecs().registerComponent<ModelComponent>();
	mEngine.ecs().registerComponent<PhysicsBodyComponent>();
	mEngine.ecs().registerComponent<MoverComponent>();

	{
		const auto readSig = mEngine.ecs().getSignature<TransformComponent, ModelComponent>();
		const auto writeSig = mEngine.ecs().getSignature<>();

		mEngine.ecs().registerSystem<FrustumCullingSystem>(readSig, writeSig, mEngine, models);
	}

	{
		const auto readSig = mEngine.ecs().getSignature<ModelComponent, PhysicsBodyComponent>();
		const auto writeSig = mEngine.ecs().getSignature<TransformComponent>();

		mEngine.ecs().registerSystem<PhysicsSystem>(readSig, writeSig, mModelHandler, mWorldId);
	}

	{
		const auto readSig = mEngine.ecs().getSignature<MoverComponent>();
		const auto writeSig = mEngine.ecs().getSignature<TransformComponent>();

		mEngine.ecs().registerSystem<MoverSystem>(readSig, writeSig, mWorldId);
	}

	loadTextures();

	mEngine.uploadTexture("TEXTURE", mPixels);

	constexpr double r = 25;
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

	// character
	{
		mCharacter = mEngine.ecs().addEntity();

		TransformComponent t{};
		t.mPosition = { 0, 2.5f, 0 };
		t.mRotation = { 0, 0, 0, 0 };
		t.mScale = { 1, 1, 1 };

		MoverComponent m{};
		m.mWalkSpeed = 10.0f;

		mEngine.ecs().addComponent<TransformComponent>(mCharacter, std::move(t));
		mEngine.ecs().addComponent<MoverComponent>(mCharacter, std::move(m));
	}
}

void MyGame::run(float delta)
{
	updateCamera(delta);

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
	mEngine.ecs().updateSystem<MoverSystem>(delta);
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
	t.mRotation = { 0, 0, 0, 0 };
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

void MyGame::updateCamera(float delta)
{
	const auto& characterTransform = mEngine.ecs().getComponent<TransformComponent>(mCharacter);
	camPosition = characterTransform.mPosition;
	camRotation = characterTransform.mRotation;
	
	const glm::vec3 camForward = camRotation * glm::vec3(0.0f, 0.0f, -1.0f);
	const glm::vec3 camRight = camRotation * glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 camUp = camRotation * glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 rot(1.0f);
	rot[0] = glm::vec4(camRight, 0.0f);
	rot[1] = glm::vec4(camUp, 0.0f);
	rot[2] = glm::vec4(-camForward, 0.0f);

	const glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), camPosition) * glm::toMat4(camRotation);
	
	const auto width = static_cast<float>(mEngine.getScreenWidth());
	const auto height = static_cast<float>(mEngine.getScreenHeight());

	if (width * height > 0)
	{
		Camera ubo{};
		ubo.mView = glm::inverse(cameraTransform);
		ubo.mProj = glm::perspective(glm::radians(90.0f), width / height, 0.01f, 100'000.0f);
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
