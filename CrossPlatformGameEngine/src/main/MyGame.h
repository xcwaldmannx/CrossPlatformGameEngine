#pragma once

#include "Graphics/Core/Engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class WindowManager;

struct Camera
{
	glm::mat4 mView;
	glm::mat4 mProj;
};

class MyGame
{
public:
	MyGame(WindowManager& windowManager);

	void run(float delta);

	void cleanup();

private:
	void loadTextures();
	void loadModels();
	void createEntities();
	void createHelicopter(glm::vec3 position);
	void updateEntities(float delta);
	void updateCamera(float delta);

private:
	WindowManager& mWindowManager;
	ascen::Engine mEngine;

	std::unordered_map<uint32_t, ascen::ModelData> mModelData;

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

	glm::vec3 camUpWorld{ 0.0f, 1.0f, 0.0f };
	glm::vec3 camPosition{ 0.0f };
	glm::vec3 camRotation{ 0.0f }; // radians: x=pitch, y=yaw
	float camSpeed = 10.0f;
};