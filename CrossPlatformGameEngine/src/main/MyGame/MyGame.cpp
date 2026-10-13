#include "MyGame.h"

#include "../WindowManager/WindowManager.h"
#include "../Utility/ImageLoader/ImageLoader.h"
#include "Ecs/Components/AnimationComponent.h"

#include "Ecs/Components/ModelComponent.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Components/CameraComponent.h"
#include "Ecs/Components/PhysicsBodyComponent.h"
#include "Ecs/Components/MoverComponent.h"
#include "Ecs/Components/AnimationComponent.h"
#include "Ecs/Components/FlightControlComponent.h"

#include "Ecs/Systems/FrustumCullingSystem.h"
#include "Ecs/Systems/PhysicsSystem.h"
#include "Ecs/Systems/MoverSystem.h"
#include "Ecs/Systems/AnimationSystem.h"
#include "Ecs/Systems/FlightControlSystem.h"
#include "Ecs/Systems/CameraSystem.h"

#include <random>
#include <cmath>

MyGame::MyGame(WindowManager& windowManager) :
	mWindowManager(windowManager),
	mEngine(windowManager),
	mPipeline(mEngine)
{
	mPipeline.init();

	mEngine.reload();

	std::vector<std::pair<std::string, std::string>> modelFilenames =
	{
		{ "model_test", "assets/models/model_test.glb" },
		{ "anim_test", "assets/models/anim_test.glb" },
		{ "tentacle", "assets/models/tentacle.glb" },
		{ "minigun", "assets/models/minigun.glb" },
		{ "uh-1y", "assets/models/uh-1y.glb" },
	};

	for (const auto& [name, filename] : modelFilenames)
	{
		mModelHandler.load(name, filename);
	}

	const auto& models = mModelHandler.getModels();
	const auto& vertices = mModelHandler.getVertices();
	const auto& indices = mModelHandler.getIndices();

	mEngine.resizeBuffer("BUFFER_VERTEX", vertices.size());
	mEngine.uploadBuffer("BUFFER_VERTEX", &vertices[0], vertices.size(), sizeof(mal::model::Vertex), 0);

	mEngine.resizeBuffer("BUFFER_INDEX", indices.size());
	mEngine.uploadBuffer("BUFFER_INDEX", &indices[0], indices.size(), sizeof(uint32_t), 0);

	// init physics world
	b3WorldDef worldDef = b3DefaultWorldDef();
	worldDef.gravity = { 0.0f, -10.0f, 0.0f };
	mWorldId  = b3CreateWorld(&worldDef);

	// initialize ECS
	mEngine.ecs().registerComponent<TransformComponent>();
	mEngine.ecs().registerComponent<ModelComponent>();
	mEngine.ecs().registerComponent<CameraComponent>();
	mEngine.ecs().registerComponent<PhysicsBodyComponent>();
	mEngine.ecs().registerComponent<MoverComponent>();
	mEngine.ecs().registerComponent<AnimationComponent>();
	mEngine.ecs().registerComponent<FlightControlComponent>();

	{
		const auto readSig = mEngine.ecs().getSignature<TransformComponent, ModelComponent>();
		const auto writeSig = mEngine.ecs().getSignature<>();

		mEngine.ecs().registerSystem<FrustumCullingSystem>(readSig, writeSig, mEngine, models);
	}

	{
		const auto readSig = mEngine.ecs().getSignature<TransformComponent>();
		const auto writeSig = mEngine.ecs().getSignature<CameraComponent>();

		mEngine.ecs().registerSystem<CameraSystem>(readSig, writeSig, mEngine);
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

	{
		const auto readSig = mEngine.ecs().getSignature<ModelComponent>();
		const auto writeSig = mEngine.ecs().getSignature<AnimationComponent>();

		mEngine.ecs().registerSystem<AnimationSystem>(readSig, writeSig, mEngine, mModelHandler.getPlayers());
	}

	{
		const auto readSig = mEngine.ecs().getSignature<TransformComponent>();
		const auto writeSig = mEngine.ecs().getSignature<FlightControlComponent, PhysicsBodyComponent>();

		mEngine.ecs().registerSystem<FlightControlSystem>(readSig, writeSig);
	}

	loadTextures();

	mEngine.uploadTexture("TEXTURE", mPixels);

	constexpr double radius = 8192.0;

	std::random_device rd;
	std::mt19937 rng(rd());

	std::uniform_real_distribution<double> angleDist(0.0, 2.0 * M_PI);
	std::uniform_real_distribution<double> radiusDist(0.0, 1.0);

	for (uint32_t i = 0; i < mEntityCount; ++i)
	{
		const double angle = angleDist(rng);
		const double distance = radius * std::sqrt(radiusDist(rng));

		const double x = distance * std::cos(angle);
		const double z = distance * std::sin(angle);

		const float rotation = static_cast<float>(-angle);

		if (i < mEntityCount / 2)
		{
			createModel("minigun", { x, 3, z },
						{ rotation, 0, 1, 0 }, { 1, 1, 1 });
		}
		else
		{
			createModel("tentacle", { x, 3, z },
						{ rotation, 0, 1, 0 }, { 2, 2, 2 });
		}
	}

	mEntityCount += 2;

	{ // helicopter
		mHelicopter = mEngine.ecs().addEntity();

		TransformComponent transform{};
		transform.mPosition = { 0.0f, 10.0f, 0 };
		transform.mRotation = { 0, 0, 0, 1 };
		transform.mScale = { 1, 1, 1 };

		ModelComponent model{};
		model.mName = "uh-1y";

		AnimationComponent anim{};
		anim.mCurrentAnimation = "idle";

		CameraComponent camera{};
		camera.mOffsetPosition = { 0, 60, 120 };
		camera.mOffsetRotation = glm::vec3 { glm::radians(-15.0f), 0, 0 };
		camera.mIsActive = false;

		PhysicsBodyComponent physicsBody{};
		physicsBody.mBodyType = DYNAMIC;
		physicsBody.mDensity = 1.0f;

		FlightControlComponent fcc;
		fcc.mType = AircraftType::HUEY;
		fcc.mIsControlled = false;

		mEngine.ecs().addComponent<TransformComponent>(mHelicopter, std::move(transform));
		mEngine.ecs().addComponent<ModelComponent>(mHelicopter, std::move(model));
		mEngine.ecs().addComponent<AnimationComponent>(mHelicopter, std::move(anim));
		mEngine.ecs().addComponent<CameraComponent>(mHelicopter, std::move(camera));
		mEngine.ecs().addComponent<PhysicsBodyComponent>(mHelicopter, std::move(physicsBody));
		mEngine.ecs().addComponent<FlightControlComponent>(mHelicopter, std::move(fcc));
	}

	{ // character
		mCharacter = mEngine.ecs().addEntity();

		TransformComponent t{};
		t.mPosition = { 0, 2.5f, -50.0f };
		t.mRotation = { 0, 0, 0, 0 };
		t.mScale = { 1, 1, 1 };

		CameraComponent camera{};
		camera.mIsActive = true;

		MoverComponent m{};
		m.mWalkSpeed = 10.0f;
		m.mIsActive = true;

		mEngine.ecs().addComponent<TransformComponent>(mCharacter, std::move(t));
		mEngine.ecs().addComponent<CameraComponent>(mCharacter, std::move(camera));
		mEngine.ecs().addComponent<MoverComponent>(mCharacter, std::move(m));
	}
}

float bladeSpin = 0;

void MyGame::run(const float delta)
{
	updateMousePicking();

	uint32_t selectedEntity = 0;
	mEngine.downloadTransfer<uint32_t>("FRAMEPASS_MOUSE_PICKING_TRANSFER", 0, &selectedEntity);
	selectedEntity--;

	if (selectedEntity < ENTITY_MAX)
	{
		auto& selectedModel = mEngine.ecs().getComponent<ModelComponent>(selectedEntity);
		selectedModel.mIsSelected = 1;

		if (selectedEntity == mHelicopter)
		{
			if (InputManager::isKeyJustPressed(GLFW_KEY_E))
			{
				auto& heloCamera = mEngine.ecs().getComponent<CameraComponent>(selectedEntity);
				heloCamera.mIsActive = !heloCamera.mIsActive;

				auto& heloFcc = mEngine.ecs().getComponent<FlightControlComponent>(selectedEntity);
				heloFcc.mIsControlled = !heloFcc.mIsControlled;

				auto& playerCamera = mEngine.ecs().getComponent<CameraComponent>(mCharacter);
				playerCamera.mIsActive = !playerCamera.mIsActive;

				auto& playerMover = mEngine.ecs().getComponent<MoverComponent>(mCharacter);
				playerMover.mIsActive = !playerMover.mIsActive;
			}
		}
		else if (InputManager::isButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			auto& selectedPhysicsBody = mEngine.ecs().getComponent<PhysicsBodyComponent>(selectedEntity);
			selectedPhysicsBody.mAppliedForce += glm::vec3{ 0, 1000, 0 };
		}
	}

	auto& heloFcc = mEngine.ecs().getComponent<FlightControlComponent>(mHelicopter);
	auto& heloAnimation = mEngine.ecs().getComponent<AnimationComponent>(mHelicopter);
	if (heloFcc.mIsControlled)
	{
		bladeSpin += 500.0f * delta;
		auto& heloPlayer = mModelHandler.getPlayer("uh-1y");
		glm::mat4 main = glm::mat4(1);
		main = glm::rotate(main, glm::radians(bladeSpin), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 tail = glm::mat4(1);
		tail = glm::rotate(main, glm::radians(bladeSpin), glm::vec3(0.0f, 1.0f, 0.0f));

		heloPlayer.setNodeTransform("main_rotor", main);
		heloPlayer.setNodeTransform("tail_rotor", tail);
	}
	else
	{
		bladeSpin = 0;
	}

	mEngine.ecs().updateSystem<CameraSystem>(delta);
	mEngine.ecs().updateSystem<MoverSystem>(delta);
	mEngine.ecs().updateSystem<FlightControlSystem>(delta);
	mEngine.ecs().updateSystem<PhysicsSystem>(delta);
	mEngine.ecs().updateSystem<AnimationSystem>(delta);
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

void MyGame::createModel(const std::string& model, const glm::vec3 pos, const glm::quat rot, const glm::vec3 scale)
{
	const auto e = mEngine.ecs().addEntity();

	TransformComponent t{};
	t.mPosition = pos;
	t.mRotation = rot;
	t.mScale = scale;

	ModelComponent m{};
	m.mName = model;
	m.mModelId = 0;
	m.mTextureId = 0;
	m.mIsHidden = false;

	PhysicsBodyComponent p{};
	p.mBodyType = DYNAMIC;
	p.mDensity = 3.0f;

	AnimationComponent anim{};
	if (model == "tentacle")
	{
		const float r = rand() % 15;

		if (r < 5.0f)
		{
		    anim.mCurrentAnimation = "dead";
		}
		else if (r >= 5.0f && r < 10.0f)
		{
		    anim.mCurrentAnimation = "idle";
		}
		else
		{
		    anim.mCurrentAnimation = "attack";
		}
	}
	else if (model == "anim_test")
	{
		anim.mCurrentAnimation = "wobble";
	}
	else if (model == "minigun")
	{
		anim.mCurrentAnimation = "rotate";
	}
	else if (model == "uh-1y")
	{
		anim.mCurrentAnimation = "idle";

		FlightControlComponent fcc;
		fcc.mType = AircraftType::HUEY;
		fcc.mIsControlled = true;
		mEngine.ecs().addComponent<FlightControlComponent>(e, std::move(fcc));
	}

	mEngine.ecs().addComponent<TransformComponent>(e, std::move(t));
	mEngine.ecs().addComponent<ModelComponent>(e, std::move(m));
	mEngine.ecs().addComponent<PhysicsBodyComponent>(e, std::move(p));

	if (model != "model_test") mEngine.ecs().addComponent<AnimationComponent>(e, std::move(anim));
}

void MyGame::updateMousePicking()
{
	double mouseX = InputManager::getMouseX();
	double mouseY = InputManager::getMouseY();

	if (WindowManager::isMouseFocused())
	{
		mouseX = WindowManager::getCenterX();
		mouseY = WindowManager::getCenterY();
	}

	const auto windowWidth = static_cast<int32_t>(WindowManager::getWidth());
	const auto windowHeight = static_cast<int32_t>(WindowManager::getHeight());

	const int32_t pixelX = std::clamp(static_cast<int32_t>(mouseX), 0, windowWidth - 1);
	const int32_t pixelY = std::clamp(static_cast<int32_t>(mouseY), 0, windowHeight - 1);

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
