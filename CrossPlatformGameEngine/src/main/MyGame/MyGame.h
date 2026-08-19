#pragma once

#include "../Engine/Core/Engine.h"

#include "ModelHandler/ModelHandler.h"
#include "MyPipeline/MyPipeline.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <box3d/box3d.h>

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
	void createModel(const std::string& model, const glm::vec3 position, const glm::vec3 scale);
	void createPhysicsWorld();
	void updateCamera(float delta);
	void updateMousePicking();

private:
	WindowManager& mWindowManager;
	ascen::Engine mEngine;

	ModelHandler mModelHandler;
	MyPipeline mPipeline;

	std::vector<float> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<float> mTransforms;
	std::vector<glm::vec3> mBoundingBoxes;

	std::vector<unsigned char> mPixels;

	enum MyModels : uint32_t
	{
		NONE = 0,
		PRISM = 1,
		SHAPES = 2,
		WINDMILL = 3,
		HELICOPTER = 4,
		FROSTY = 5,
		TEST = 6,
	};

	std::vector<std::pair<uint32_t, std::string>> mModelFilepaths =
	{
		{ PRISM,      "res/models/prism.model"      },
		{ SHAPES,     "res/models/shapes.model"     },
		{ WINDMILL,   "res/models/windmill.model"   },
		{ HELICOPTER, "res/models/helicopter.model" },
		{ FROSTY,     "res/models/frosty.model"     },
	};

	double mEntityCount = 16;

	b3WorldId mWorldId;
	EntityId mCharacter;

	glm::vec3 camUpWorld { 0.0f, 1.0f, 0.0f };
	glm::vec3 camPosition { 0.0f, 5.0f, 0.0f };
	glm::quat camRotation;
	float camSpeed = 15.0f;
};