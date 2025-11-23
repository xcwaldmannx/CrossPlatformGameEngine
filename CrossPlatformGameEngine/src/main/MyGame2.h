#pragma once

#include "Graphics/Engine.h"

#include <glm/glm.hpp>

class WindowManager;

struct Camera
{
	glm::mat4 mView;
	glm::mat4 mProj;
};

class MyGame2
{
public:
	MyGame2(WindowManager& windowManager);

	void run(float delta);

	void cleanup();

private:
	void loadTextures();
	void loadModels();

private:
	WindowManager& mWindowManager;
	ascen::Engine mEngine;

	std::unordered_map<uint32_t, ModelData> mModelData;

	std::vector<float> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<float> mTransforms;

	std::vector<unsigned char> mPixels;

	enum MyModels : uint32_t
	{
		NONE = 0,
		PRISM = 1,
		SHAPES = 2,
		WINDMILL = 3,
		HELICOPTER = 4,
		FROSTY = 5,
	};

	std::vector<std::pair<uint32_t, std::string>> mModelFilepaths =
	{
		{ PRISM,      "res/models/prism.model"      },
		{ SHAPES,     "res/models/shapes.model"     },
		{ WINDMILL,   "res/models/windmill.model"   },
		{ HELICOPTER, "res/models/helicopter.model" },
		{ FROSTY,     "res/models/frosty.model"     },
	};
};