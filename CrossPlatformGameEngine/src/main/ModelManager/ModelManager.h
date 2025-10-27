#pragma once

#include "../BadGraphicsPipeline/Vertex/Vertex.h"

#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

struct ModelData {
	size_t mVertexOffset    = 0;
	size_t mVertexCount     = 0;
	size_t mIndexOffset     = 0;
	size_t mIndexCount      = 0;
	size_t mTransformOffset = 0;
};

struct Model {
	ModelData mMesh;
	std::unordered_map<std::string, ModelData> mMeshes;
};

class ModelManager {
public:
	void createModel(std::string name, const char* filepath);

	const Model& getModel(std::string key) const;
	const Model& getModel(int id) const;
	int getModelId(std::string key) const;
	const std::unordered_map<std::string, Model>& getModels() const;

	const std::vector<Vertex>& getVertices() const;
	const std::vector<uint32_t>& getIndices() const;
	const std::vector<glm::mat4>& getTransforms() const;

private:
	void processModel(aiNode* node, aiMesh** meshes, Model& mesh);
	void processNodes(aiNode* node, aiMesh** meshes, Model& mesh);

private:
	std::unordered_map<std::string, Model> mKeyToModel;
	std::unordered_map<int, Model&> mIdToModel;
	std::unordered_map<std::string, int> mKeyToId;

	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<glm::mat4> mTransforms;

	int mModelCount = 0;
};