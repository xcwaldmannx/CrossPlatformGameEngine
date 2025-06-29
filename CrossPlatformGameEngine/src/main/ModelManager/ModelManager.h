#pragma once

#include "../GraphicsPipeline/Vertex/Vertex.h"

#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

struct ModelMesh {
	size_t mVertexOffset    = 0;
	size_t mVertexCount     = 0;
	size_t mIndexOffset     = 0;
	size_t mIndexCount      = 0;
	size_t mTransformOffset = 0;
};

struct Model {
	size_t mVertexOffset    = 0;
	size_t mVertexCount     = 0;
	size_t mIndexOffset     = 0;
	size_t mIndexCount      = 0;
	size_t mTransformOffset = 0;
	std::unordered_map<std::string, ModelMesh> mMeshes;
};

class ModelManager {
public:
	void createModel(std::string name, const char* filepath);

	const Model& getModel(std::string name) const;
	const std::unordered_map<std::string, Model>& getModels() const;

	const std::vector<Vertex>& getVertices() const;
	const std::vector<uint32_t>& getIndices() const;
	const std::vector<glm::mat4>& getTransforms() const;

private:
	void processModel(aiNode* node, aiMesh** meshes, Model& mesh);
	void processNodes(aiNode* node, aiMesh** meshes, Model& mesh);

private:
	std::unordered_map<std::string, Model> mModels;

	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<glm::mat4> mTransforms;
};