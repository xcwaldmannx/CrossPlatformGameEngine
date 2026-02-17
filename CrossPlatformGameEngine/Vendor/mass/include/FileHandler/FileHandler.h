#pragma once

#include "../Configuration.h"
#include "../Model/Model.h"

#include <string>

#include <assimp/scene.h>
#include <nlohmann/json.hpp>

namespace mass
{

	class FileHandler
	{
	public:
		Model readModel(const Configuration& config, const std::string& filepath);
		nlohmann::json readSerialized(const Configuration& config, const std::string& filepath);

		void write(const std::string& data, const std::string& filepath);

	private:
		void processNodes(aiNode* node, aiMesh** meshes, Model& model, const Configuration& config);
	};

}
