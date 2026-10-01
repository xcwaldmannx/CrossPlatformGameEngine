#include "../ModelHandler/ModelHandler.h"

ModelHandler::ModelHandler() {}

const Model& ModelHandler::getModel(const std::string& name)
{
    if (mModels.contains(name)) return mModels.at(name);
    throw std::runtime_error("model does not exist");
}

void ModelHandler::load(const std::string& name, const std::string &filename)
{
    mal::Configuration config{};
    config.mSupportedFileExtensions = { "glb" };

    const mal::model::Model model = mal::ModelHandler::load(filename, config);

    const auto& vertices = model.getVertices();
    const auto& indices = model.getIndices();

    mModels.emplace(name, Model());
    Model& m = mModels.at(name);

    m.mId = std::hash<std::string>()(name);
    m.mModel = model;
    m.mVertexOffset = sGlobalVertexOffset;
    m.mIndexOffset = sGlobalIndexOffset;
    m.mIndexCount = indices.size();
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

    sGlobalVertexOffset = mVertices.size();
    sGlobalIndexOffset = mIndices.size();
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

const std::vector<mal::model::Vertex>& ModelHandler::getVertices()
{
    return mVertices;
}
const std::vector<uint32_t>& ModelHandler::getIndices()
{
    return mIndices;
}
