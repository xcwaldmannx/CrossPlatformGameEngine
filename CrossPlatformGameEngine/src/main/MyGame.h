#pragma once

#include "WindowManager/WindowManager.h"
#include "Graphics/Engine.h"
#include "Graphics/Renderer.h"

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Mass.h>

class MyGame
{
public:
	struct Vertex : public ascen::Vertex_I
	{
		glm::vec3 mPosition;
		glm::vec3 mNormal;
		glm::vec2 mTexCoord;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription{};

			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
			attributeDescriptions.resize(3);

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, mPosition);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, mNormal);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, mTexCoord);

			return attributeDescriptions;
		}
	};

	struct Camera
	{
		glm::mat4 mView;
		glm::mat4 mProj;
	};

	MyGame(WindowManager& windowManager);

	void run(float delta);

	void destroy();

private:
	void createTextures();

	void loadModels();

	void createEntities();
	void updateEntities(float delta);

	void updateCamera();

private:
	WindowManager mWindowManager;
	ascen::Engine mEngine;
	ascen::Renderer mRenderer;

	std::unordered_map<uint32_t, ModelData> mModelData;

	std::vector<float> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<float> mTransforms;

	enum MyModels : uint32_t
	{
		NONE       = 0,
		PRISM      = 1,
		SHAPES     = 2,
		WINDMILL   = 3,
		HELICOPTER = 4,
		FROSTY     = 5,
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