#pragma once

#include <stdexcept>

#include <string>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct ModelData
{
	size_t mVertexOffset = 0;
	size_t mVertexCount = 0;
	size_t mIndexOffset = 0;
	size_t mIndexCount = 0;
	size_t mTransformOffset = 0;
};

struct Model
{
	ModelData mMesh;
	std::unordered_map<std::string, ModelData> mMeshes;
};

using NameModelMap = std::unordered_map<std::string, Model>;

template<typename T>
class ModelLoader
{
public:
	static void load(const std::string name, const char* filepath, NameModelMap& map)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcessPreset_TargetRealtime_MaxQuality /*aiProcess_FlipUVs*/);

		if (!scene) {
			throw std::runtime_error("Could not load model!");
		}

		auto meshes = scene->mMeshes;
		auto rootNode = scene->mRootNode;

		if (map.find(name) != map.end()) {
			throw std::runtime_error("Model with name " + name + " already exists!");
		}

		Model model;

		processModel(rootNode, meshes, model);
		map.emplace(name, std::move(model));
	}

protected:
	static void processModel(aiNode* node, aiMesh** meshes, Model& mesh);
	static void processNodes(aiNode* node, aiMesh** meshes, Model& mesh);

};