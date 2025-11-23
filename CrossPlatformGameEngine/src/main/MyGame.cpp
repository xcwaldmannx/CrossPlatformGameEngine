#include "MyGame.h"
/*
#include "Utility/ImageLoader/ImageLoader.h"

#include "Graphics/Ecs/Components/TransformComponent.h"
#include "Graphics/Ecs/Components/ModelComponent.h"

#include "Graphics/Ecs/Systems/RenderSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

MyGame::MyGame(WindowManager& windowManager) :
	mEngine(windowManager),
	mRenderer(windowManager, mEngine)
{
	loadModels();
	mRenderer.updateModels(mModelData);

	std::cout << "vertices: " << mVertices.size() << "\n";
	std::cout << "indices: " << mIndices.size() << "\n";
	std::cout << "transforms: " << mTransforms.size() << "\n";

	mRenderer.createVertexBuffer("vertices", mVertices);
	mRenderer.createIndexBuffer("indices", mIndices);
	mRenderer.createUniformBuffer<Camera>("camera");
	mRenderer.createStorageBuffer<float>("transforms", mTransforms);
	mRenderer.createSampler("sampler");

	createTextures();

	std::cout << "created resources\n";

	mRenderer.addResourceLayout(
		{ "set0", "camera", 0x00, sizeof(Camera), ascen::Renderer::UBO_DYNAMIC, ascen::Renderer::VERTEX });
	mRenderer.addResourceLayout(
		{ "set0", "transforms", 0x01, VK_WHOLE_SIZE, ascen::Renderer::SSBO, ascen::Renderer::VERTEX });
	mRenderer.addResourceLayout(
		{ "set0", "sampler", 0x10, 0, ascen::Renderer::SAMPLER, ascen::Renderer::PIXEL });
	mRenderer.addResourceLayout(
		{ "set0", "textures", 0x11, 0, ascen::Renderer::IMAGE, ascen::Renderer::PIXEL });

	mRenderer.finalize();

	std::cout << "created descriptors\n";

	mRenderer.createGraphicsPipeline<Vertex>(
		"graphics", { "set0" }, "src/shaders/GPUDrivenVS.spv", "src/shaders/GPUDrivenPS.spv");
	mRenderer.createComputePipeline("compute", { "set0" }, "src/shaders/GPUDrivenCS.spv");

	std::cout << "created pipelines\n";

	ascen::Renderer::Pass pass{};
	pass.mVertexBuffers = { "vertices" };
	pass.mIndexBuffer = "indices";
	pass.mGraphicsPipeline = "graphics";
	pass.mDescriptorSets = { "set0" };

	mRenderer.createRenderPass("scene", pass);

	// mRenderer.finalize(); this should go here at the very end, but doesn't work with pipeline creation yet

	createEntities();

	std::cout << "Done init\n";
}

void MyGame::run(float delta)
{
	if (mWindowManager.isResized())
	{
		mRenderer.resize();
		mWindowManager.setResized(false);
	}


	updateEntities(delta);
	updateCamera();

	mRenderer.drawFrame();
}

void MyGame::destroy()
{

	mRenderer.cleanup();
	mEngine.cleanup();

}

void MyGame::createTextures()
{
	ImageLoader il;

	std::vector<const char*> mTextureFilepaths =
	{
		"res/textures/testimg1.png",
		"res/textures/testimg2.png",
		"res/textures/testimg3.png",
	};

	std::vector<unsigned char> pixels;
	uint32_t width = 1024;
	uint32_t height = 1024;
	uint32_t channels = 4;

	pixels.reserve(width * height * channels * mTextureFilepaths.size());

	for (const auto& filepath : mTextureFilepaths)
	{
		RawImage raw;
		il.loadImage(filepath, &raw);
		pixels.insert(pixels.end(), raw.mPixels.begin(), raw.mPixels.end());
	}

	mRenderer.createImageTexture("textures", pixels, width, height, mTextureFilepaths.size());
}

void MyGame::loadModels()
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

void MyGame::createEntities()
{
	{
		auto e = mEngine.ecs().addEntity();

		TransformComponent t{};
		t.mPosition = { 0, 0, -10 };
		t.mRotation = { 0.25, 0, 0 };
		t.mScale    = { 1, 1, 1 };

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

void MyGame::updateEntities(float delta)
{
	{ // helicopter
		auto& t = mEngine.ecs().getComponent<TransformComponent>(0);
		t.mRotation += glm::vec3(0, 1.0f, 0) * delta;
	
		auto& m = mEngine.ecs().getComponent<ModelComponent>(0);
		m.mMeshTransforms[1].mRotation += glm::vec3(0, 10.0f, 0) * delta; // main
		m.mMeshTransforms[2].mRotation += glm::vec3(40.0f, 0, 0) * delta; // tail
	}
}

void MyGame::updateCamera()
{
	glm::mat4 transform = glm::mat4(1.0);

	Camera ubo{};
	ubo.mView = glm::inverse(transform);
	ubo.mProj = glm::perspective(glm::radians(90.0f),
		mRenderer.getWidth() / (float) mRenderer.getHeight(), 0.01f, 100.0f);

	ubo.mProj[1][1] *= -1;

	mRenderer.updateUniformBuffer<Camera>("camera", ubo);
}

*/