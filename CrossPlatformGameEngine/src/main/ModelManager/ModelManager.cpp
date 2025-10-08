#include "ModelManager.h"

#include "../GraphicsPipeline/Vertex/Vertex.h"

#include <iostream>
#include <stdexcept>

void ModelManager::createModel(std::string name, const char* filepath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath,
        aiProcessPreset_TargetRealtime_MaxQuality /*aiProcess_FlipUVs*/);

    if (!scene) {
        throw std::runtime_error("Could not load model!");
    }

    auto meshes = scene->mMeshes;
    auto rootNode = scene->mRootNode;

    if (mKeyToModel.find(name) != mKeyToModel.end()) {
        throw std::runtime_error("Model with name " + name + " already exists!");
    }

    Model model;

    processModel(rootNode, meshes, model);

    mKeyToModel.emplace(name, std::move(model));
    mIdToModel.emplace(mModelCount, mKeyToModel.at(name));
    mKeyToId.emplace(name, mModelCount);
    mModelCount++;
}

const Model& ModelManager::getModel(std::string key) const
{
    return mKeyToModel.at(key);
}

const Model& ModelManager::getModel(int id) const
{
    return mIdToModel.at(id);
}

int ModelManager::getModelId(std::string key) const
{
    return mKeyToId.at(key);
}

const std::unordered_map<std::string, Model>& ModelManager::getModels() const
{
    return mKeyToModel;
}

const std::vector<Vertex>& ModelManager::getVertices() const
{
    return mVertices;
}

const std::vector<uint32_t>& ModelManager::getIndices() const
{
    return mIndices;
}

const std::vector<glm::mat4>& ModelManager::getTransforms() const
{
    return mTransforms;
}

void ModelManager::processModel(aiNode* node, aiMesh** meshes, Model& model)
{
    size_t vertexOffset = mVertices.size();
    size_t indexOffset = mIndices.size();
    size_t transformOffset = mTransforms.size();

    model.mMesh.mVertexOffset = vertexOffset;
    model.mMesh.mIndexOffset = indexOffset;
    model.mMesh.mTransformOffset = transformOffset;

    processNodes(node, meshes, model);
}

void ModelManager::processNodes(aiNode* node, aiMesh** meshes, Model& model)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        auto mesh = meshes[node->mMeshes[i]];

        if (!mesh->HasPositions())
            continue;

        size_t vertexOffset = mVertices.size();
        size_t indexOffset = mIndices.size();
        size_t transformOffset = mTransforms.size();

        // Extract vertices
        for (unsigned int vIndex = 0; vIndex < mesh->mNumVertices; vIndex++)
        {
            Vertex vert{};
            vert.mPosition = glm::vec3(
                mesh->mVertices[vIndex].x,
                mesh->mVertices[vIndex].y,
                mesh->mVertices[vIndex].z
            );

            if (mesh->HasNormals())
            {
                vert.mNormal = glm::vec3(
                    mesh->mNormals[vIndex].x,
                    mesh->mNormals[vIndex].y,
                    mesh->mNormals[vIndex].z
                );
            }

            if (mesh->HasTextureCoords(0))
            {
                vert.mTexCoord = glm::vec2(
                    mesh->mTextureCoords[0][vIndex].x,
                    mesh->mTextureCoords[0][vIndex].y
                );
            }

            mVertices.emplace_back(std::move(vert));
        }

        // Extract indices
        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            const aiFace& face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                uint32_t index = face.mIndices[j] + vertexOffset;
                mIndices.emplace_back(std::move(index));
            }
        }

        uint32_t vertexCount = mesh->mNumVertices;
        uint32_t indexCount = mesh->mNumFaces * 3;

        if (vertexOffset + vertexCount >= UINT32_MAX)
        {
            throw std::runtime_error("Too many vertices for uint16_t indexing.");
        }

        model.mMesh.mVertexCount += vertexCount;
        model.mMesh.mIndexCount += indexCount;

        // Transform
        auto t = node->mTransformation;

        glm::mat4 transform =
        {
            { t.a1, t.b1, t.c1, t.d1 },
            { t.a2, t.b2, t.c2, t.d2 },
            { t.a3, t.b3, t.c3, t.d3 },
            { t.a4, t.b4, t.c4, t.d4 },
        };

        mTransforms.emplace_back(std::move(transform));

        // Store mesh instance
        std::cout << "Loaded model " << mesh->mName.C_Str() << "\n";
        model.mMeshes[mesh->mName.C_Str()] =
        {
            .mVertexOffset = vertexOffset,
            .mVertexCount = vertexCount,
            .mIndexOffset = indexOffset,
            .mIndexCount = indexCount,
            .mTransformOffset = transformOffset,
        };
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNodes(node->mChildren[i], meshes, model);
    }
}
