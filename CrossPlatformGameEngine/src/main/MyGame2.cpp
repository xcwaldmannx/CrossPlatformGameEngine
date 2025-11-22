#include"MyGame2.h"

#include "Utility/ImageLoader/ImageLoader.h"

#include <Mass.h>

MyGame2::MyGame2(WindowManager& windowManager) :
	mWindowManager(windowManager),
	mEngine(windowManager)
{
	const ascen::VertexBinding binding { 0, 32, VK_VERTEX_INPUT_RATE_VERTEX };
	const std::vector<ascen::VertexAttribute> attributes =
	{
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
		{ 0, 1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3 },
		{ 0, 2, VK_FORMAT_R32G32_SFLOAT, sizeof(float) * 6 }
	};

	mEngine.vertex().registerVertex({ "simpleVertex", binding, attributes });

	mEngine.vertex().reconstruct();

	mEngine.resource().registerBuffer({ "vertex", ascen::BufferType::VERTEX, 1'000'000, 8 * sizeof(float) });
	mEngine.resource().registerBuffer({ "index", ascen::BufferType::INDEX, 1'000'000, sizeof(uint32_t) });
	mEngine.resource().registerBuffer({ "camera", ascen::BufferType::UNIFORM, 2, sizeof(Camera) });
	mEngine.resource().registerBuffer({ "transform", ascen::BufferType::STORAGE, 1'000'000, sizeof(float) });

	mEngine.resource().registerSampler({ "sampler" });
	mEngine.resource().registerTexture({ "texture", ascen::TextureType::IMAGE, 1024, 1024, 16 });

	mEngine.resource().reconstruct();

	mEngine.descriptor().registerDescriptor(
		{ "set0", "camera", 0x00, sizeof(Camera), ascen::DescriptorType::SSBO, ascen::DescriptorStage::VERTEX});
	mEngine.descriptor().registerDescriptor(
		{ "set0", "transform", 0x01, sizeof(float), ascen::DescriptorType::SSBO, ascen::DescriptorStage::VERTEX});
	mEngine.descriptor().registerDescriptor(
		{ "set0", "sampler", 0x10, 0, ascen::DescriptorType::SSBO, ascen::DescriptorStage::PIXEL });
	mEngine.descriptor().registerDescriptor(
		{ "set0", "texture", 0x11, 0, ascen::DescriptorType::SSBO, ascen::DescriptorStage::PIXEL });

	mEngine.descriptor().reconstruct();

	mEngine.pipeline().registerGraphicsPipeline(
		{ "pipeline", "src/shaders/GPUDrivenVS.spv", "src/shaders/GPUDrivenPS.spv", "simpleVertex", { "set0" }});

	mEngine.pipeline().reconstruct();

	mEngine.frame().registerFramePass(
		{
			"frame", { "vertex" }, "index", {}, {}, {}, {}, { "set0" }, "", ascen::FramePassType::GRAPHICS
		});

	mEngine.frame().reconstruct();

	loadModels();

	mEngine.resource().updateBuffer("vertex", mVertices.data(), mVertices.size(), sizeof(float));
	mEngine.resource().updateBuffer("index", mIndices.data(), mIndices.size(), sizeof(uint32_t));
	mEngine.resource().updateBuffer("transform", mTransforms.data(), mTransforms.size(), sizeof(float));

	loadTextures();

	mEngine.resource().updateTexture("texture", mPixels);
}

void MyGame2::run(float delta)
{
	while (mWindowManager.isRunning())
	{

	}

	cleanup();
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
