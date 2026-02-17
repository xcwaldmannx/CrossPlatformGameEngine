#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

namespace mass
{

	struct Vertex
	{
		glm::vec3 mPosition{};
		glm::vec3 mNormal{};
		glm::vec4 mColor{};
		glm::vec2 mTexCoord{};
	};

	struct Animation
	{
		// TODO: add animations
	};

	struct Mesh
	{
		size_t mVertexOffset = 0;
		size_t mVertexCount = 0;

		size_t mIndexOffset = 0;
		size_t mIndexCount = 0;

		size_t mTransformOffset = 0;
	};

	struct Model
	{
		std::unordered_map<std::string, Mesh> mMeshes;
		std::vector<Vertex> mVertices;
		std::vector<uint32_t> mIndices;
		std::vector<glm::mat4> mTransforms;
		std::vector<Animation> mAnimations;
	};

}
