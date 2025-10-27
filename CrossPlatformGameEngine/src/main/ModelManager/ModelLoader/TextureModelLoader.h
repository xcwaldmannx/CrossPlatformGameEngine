#pragma once

#include "ModelLoader.h"

#include "../../Graphics/Vertex/TextureVertex.h"

class TextureModelLoader : public ModelLoader<ascen::TextureVertex>
{
private:
	static void processModel(aiNode* node, aiMesh** meshes, Model& mesh);
	static void processNodes(aiNode* node, aiMesh** meshes, Model& mesh);
};