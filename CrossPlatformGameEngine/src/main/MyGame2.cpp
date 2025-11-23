#include"MyGame2.h"

#include "WindowManager/WindowManager.h"
#include "Utility/ImageLoader/ImageLoader.h"

#include <Mass.h>

MyGame2::MyGame2(WindowManager& windowManager) :
	mWindowManager(windowManager),
	mEngine(windowManager)
{
	const ascen::VertexBinding binding { 0, sizeof(float) * 8, VK_VERTEX_INPUT_RATE_VERTEX};
	const std::vector<ascen::VertexAttribute> attributes =
	{
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
		{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3 },
		{ 2, 0, VK_FORMAT_R32G32_SFLOAT, sizeof(float) * 6 }
	};

	mEngine.vertex().registerVertex({ "simpleVertex", binding, attributes });

	mEngine.resource().registerBuffer({ "vertex", ascen::BufferType::VERTEX, 1'000'000, sizeof(float) * 8 });
	mEngine.resource().registerBuffer({ "index", ascen::BufferType::INDEX, 1'000'000, sizeof(uint32_t) });
	mEngine.resource().registerBuffer({ "camera", ascen::BufferType::UNIFORM, 2, sizeof(Camera) });
	mEngine.resource().registerBuffer({ "transform", ascen::BufferType::STORAGE, 1'000'000, sizeof(float) });

	mEngine.resource().registerSampler({ "sampler" });
	mEngine.resource().registerTexture({ "texture", ascen::TextureType::IMAGE, 1024, 1024, 16 });

	mEngine.descriptor().registerDescriptor(
		{ "camera", "set0", 0x00, sizeof(Camera), ascen::DescriptorType::UBO_DYNAMIC, ascen::DescriptorStage::VERTEX});
	mEngine.descriptor().registerDescriptor(
		{ "transform", "set0", 0x01, VK_WHOLE_SIZE, ascen::DescriptorType::SSBO, ascen::DescriptorStage::VERTEX});
	mEngine.descriptor().registerDescriptor(
		{ "sampler", "set0", 0x10, 0, ascen::DescriptorType::SAMPLER, ascen::DescriptorStage::PIXEL });
	mEngine.descriptor().registerDescriptor(
		{ "texture", "set0", 0x11, 0, ascen::DescriptorType::IMAGE, ascen::DescriptorStage::PIXEL });

	mEngine.pipeline().registerGraphicsPipeline(
		{ "pipeline", "src/shaders/GPUDrivenVS.spv", "src/shaders/GPUDrivenPS.spv", "simpleVertex", { "set0" }});

	mEngine.frame().registerFramePass(
		{
			"frame", { "vertex" }, "index", {}, {}, {}, {}, { "set0" }, "pipeline", ascen::FramePassType::GRAPHICS
		});

	mEngine.reload();

	loadModels();

	mEngine.resource().updateBuffer("vertex", mVertices.data(), mVertices.size(), sizeof(float));
	mEngine.resource().updateBuffer("index", mIndices.data(), mIndices.size(), sizeof(uint32_t));
	mEngine.resource().updateBuffer("transform", mTransforms.data(), mTransforms.size(), sizeof(float));

	loadTextures();

	mEngine.resource().updateTexture("texture", mPixels);

	mEngine.updateModelData(mModelData);

	// createEntities();

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			createHelicopter({ -100 + (i * 20), 0, -10 - (j * 20)});
		}
	}
}

void MyGame2::run(float delta)
{
	updateCamera(delta);
	updateEntities(delta);
	mEngine.drawFrame();
}

void MyGame2::cleanup()
{
	mEngine.cleanup();
}

void MyGame2::loadTextures()
{
	ImageLoader il;

	std::vector<const char*> mTextureFilepaths =
	{
		"res/textures/testimg1.png",
		"res/textures/testimg2.png",
		"res/textures/testimg3.png",
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

void MyGame2::loadModels()
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

		mModelData.emplace(path.first, std::move(info));

		mVertices.insert(mVertices.end(), modelLayout.mVertices.begin(), modelLayout.mVertices.end());
		mIndices.insert(mIndices.end(), modelLayout.mIndices.begin(), modelLayout.mIndices.end());
		mTransforms.insert(mTransforms.end(), modelLayout.mTransforms.begin(), modelLayout.mTransforms.end());

		globalVertexOffset = mVertices.size() / floatsPerVertex;
		globalIndexOffset = mIndices.size();
		globalTransformOffset = mTransforms.size() / 16;
	}
}

void MyGame2::createEntities()
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

void MyGame2::createHelicopter(glm::vec3 position)
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

void MyGame2::updateEntities(float delta)
{
	for (int i = 0; i < 1000; i++)
	{ // helicopters
		auto& t = mEngine.ecs().getComponent<TransformComponent>(i);
		t.mRotation += glm::vec3(0, 1.0f, 0) * delta;

		auto& m = mEngine.ecs().getComponent<ModelComponent>(i);
		m.mMeshTransforms[1].mRotation += glm::vec3(0, 10.0f, 0) * delta; // main
		m.mMeshTransforms[2].mRotation += glm::vec3(40.0f, 0, 0) * delta; // tail
	}
}

void MyGame2::updateCamera(float delta)
{
	// --- yaw (Q/E), pitch clamped ---
	if (mWindowManager.getInput().isKeyPressed(GLFW_KEY_Q)) camRotation.y -= camSpeed * 0.25f * delta; // look left
	if (mWindowManager.getInput().isKeyPressed(GLFW_KEY_E)) camRotation.y += camSpeed * 0.25f * delta; // look right
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
	
	Camera ubo{};
	ubo.mView = glm::inverse(cameraTransform);
	ubo.mProj = glm::perspective(glm::radians(90.0f),
		(float) mEngine.getScreenWidth() / (float) mEngine.getScreenHeight(), 0.01f, 10'000.0f);
	ubo.mProj[1][1] *= -1;

	mEngine.resource().updateBuffer("camera", &ubo, 1, sizeof(Camera), mEngine.getCurrentFrame());
}
