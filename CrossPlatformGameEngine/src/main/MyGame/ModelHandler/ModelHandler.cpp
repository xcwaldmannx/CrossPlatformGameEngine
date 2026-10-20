#include "../ModelHandler/ModelHandler.h"

ModelHandler::ModelHandler() {}

const Model& ModelHandler::getModel(const std::string& name)
{
    if (mModels.contains(name)) return mModels.at(name);
    throw std::runtime_error("model does not exist");
}
#include <iostream>
void ModelHandler::load(const std::string& name, const std::string &filename)
{
    mal::Configuration config{};
    config.mSupportedFileExtensions = { "glb" };

    const mal::model::Model model = mal::ModelHandler::load(filename, config);

    const auto& vertices = model.getVertices();
    const auto& indices = model.getIndices();
    const auto meshes = model.getMeshes();
    const auto& textures = model.getTextures();
    const auto& materials = model.getMaterials();

    mModels.emplace(name, Model());
    Model& m = mModels.at(name);

    m.mId = std::hash<std::string>()(name);
    m.mModel = model;
    m.mVertexOffset = sGlobalVertexOffset;
    m.mIndexOffset = sGlobalIndexOffset;
    m.mIndexCount = indices.size();
    m.mTextureOffset = sGlobalTextureOffset;
    m.mMaterialOffset = sGlobalMaterialOffset;
    m.mBoundsMax = model.getBoundsMax();
    m.mBoundsMin = model.getBoundsMin();
    m.mMeshes.append_range(meshes);

    if (model.hasAnimations())
    {
        m.mAnimationSet = mal::AnimationHandler::load(filename, config);
        auto player = mal::AnimationPlayer(m.mModel, m.mAnimationSet);
        mPlayers.emplace(name, player);
    }

    mVertices.append_range(vertices);
    mIndices.append_range(indices);
    mTextures.append_range(textures);

    for (const auto& material : materials)
    {
        mal::model::Material gpuMaterial = material;

        auto remap = [&](int32_t& textureId)
        {
            if (textureId >= 0) textureId += static_cast<int32_t>(m.mTextureOffset);
        };

        remap(gpuMaterial.mBaseColorTextureId);
        remap(gpuMaterial.mNormalTextureId);
        remap(gpuMaterial.mMetallicRoughnessTextureId);
        remap(gpuMaterial.mOcclusionTextureId);
        remap(gpuMaterial.mEmissiveTextureId);

        mMaterials.push_back(gpuMaterial);
    }

    sGlobalVertexOffset = mVertices.size();
    sGlobalIndexOffset = mIndices.size();
    sGlobalTextureOffset = mTextures.size();
    sGlobalMaterialOffset = mMaterials.size();
}

const std::unordered_map<std::string, Model>& ModelHandler::getModels()
{
    return mModels;
}

mal::AnimationPlayer& ModelHandler::getPlayer(const std::string& name)
{
    if (mPlayers.contains(name)) return mPlayers.at(name);
    throw std::runtime_error("player does not exist");
}

std::unordered_map<std::string, mal::AnimationPlayer>& ModelHandler::getPlayers()
{
    return mPlayers;
}

const mal::model::VertexArray& ModelHandler::getVertices()
{
    return mVertices;
}
const mal::model::IndexArray& ModelHandler::getIndices()
{
    return mIndices;
}

const mal::model::TextureArray& ModelHandler::getTextures()
{
    return mTextures;
}

const mal::model::MaterialArray& ModelHandler::getMaterials()
{
    return mMaterials;
}

