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

    if (model.hasAnimations())
    {
        m.mAnimationSet = mal::AnimationHandler::load(filename, config);
        auto player = mal::AnimationPlayer(m.mModel, m.mAnimationSet);
        mPlayers.emplace(name, player);
    }

    mVertices.append_range(vertices);
    mIndices.append_range(indices);
    mTextures.append_range(textures);

    std::cout << "Model: " << name << '\n';
    std::cout << "Textures: " << textures.size() << '\n';
    std::cout << "Materials: " << materials.size() << '\n';
    std::cout << "Global texture offset: " << m.mTextureOffset << '\n';

    for (size_t i = 0; i < materials.size(); ++i)
    {
        const auto& mat = materials[i];

        std::cout
            << "Material " << i
            << " baseColorTextureId: " << mat.mBaseColorTextureId
            << " normalTextureId: " << mat.mNormalTextureId
            << '\n';
    }

    for (auto material : materials)
    {
        auto remap = [&](int32_t& textureId)
        {
            if (textureId >= 0) textureId += static_cast<int32_t>(m.mTextureOffset);
        };

        remap(material.mBaseColorTextureId);
        remap(material.mNormalTextureId);
        remap(material.mMetallicRoughnessTextureId);
        remap(material.mOcclusionTextureId);
        remap(material.mEmissiveTextureId);

        mMaterials.push_back(material);
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

