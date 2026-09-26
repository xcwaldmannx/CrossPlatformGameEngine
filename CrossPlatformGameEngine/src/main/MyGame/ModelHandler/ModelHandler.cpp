#include "../ModelHandler/ModelHandler.h"

ModelHandler::ModelHandler() {}

void ModelHandler::load(const std::string& name, const std::string &filename)
{
    mal::Configuration config{};
    config.mSupportedFileExtensions = { "glb" };

    const mal::model::Model model = mal::ModelHandler::load(filename, config);
    const mal::anim::AnimationSet animationSet = mal::AnimationHandler::load(filename, config);

    Model m{};
    m.mModel = model;
    m.mAnimationSet = animationSet;
    m.mVertexOffset = mGlobalVertexOffset;
    m.mIndexOffset = mGlobalIndexOffset;
    m.mIndexCount = model.getIndices().size();

    const auto [maxX, maxY, maxZ] = model.getBoundsMax();
    m.mBoundsMax = { maxX, maxY, maxZ };

    const auto [minX, minY, minZ] = model.getBoundsMin();
    m.mBoundsMin = { minX, minY, minZ };

    mModels.emplace(name, m);

    mGlobalVertexOffset += model.getVertices().size();
    mGlobalIndexOffset += model.getIndices().size();

    mVertices.append_range(model.getVertices());
    mIndices.append_range(model.getIndices());
}

const Model& ModelHandler::getModel(const std::string& name)
{
    if (mModels.contains(name)) return mModels.at(name);
    throw std::runtime_error("model does not exist");
}

const std::unordered_map<std::string, Model>& ModelHandler::getModels()
{
    return mModels;
}

const std::vector<mal::model::Vertex>& ModelHandler::getVertices()
{
    return mVertices;
}
const std::vector<uint32_t>& ModelHandler::getIndices()
{
    return mIndices;
}

/*
void ModelHandler::loadModels(const std::vector<std::string>& filepaths)
{
    mass::Configuration config{};
    config.mVertexLayout.mAttributes =
    {
		    { 3, sizeof(float), 0 },
            { 3, sizeof(float), sizeof(float) * 3 },
            { 2, sizeof(float), sizeof(float) * 6 },
    };

    config.mHasNormals = true;
    config.mHasTexCoords = true;
    config.mHasTransforms = true;

    size_t globalVertexOffset = 0;
    size_t globalIndexOffset = 0;
    size_t globalTransformOffset = 0;
    size_t globalBoundsOffset = 0;

    for (const auto& filepath : filepaths)
    {
        const auto modelLayout = mass::deserialize(config, filepath);

        const size_t floatsPerVertex = modelLayout.mVertexLayout.mStride / sizeof(float);
        const size_t floatsPerTransform = 16;

        MyModel model{};
        model.mModelId = std::hash<std::string>()(filepath);
        model.mVertexOffset = globalVertexOffset;
        model.mIndexOffset = globalIndexOffset;
        model.mIndexCount = modelLayout.mIndices.size();
        model.mTransformOffset = globalTransformOffset;
        model.mBoundsPos = modelLayout.mBoundsPos;
        model.mBoundsNeg = modelLayout.mBoundsNeg;

        mModels[filepath] = model;

        mVertices.append_range(modelLayout.mVertices);
        mIndices.append_range(modelLayout.mIndices);
        mTransforms.append_range(modelLayout.mTransforms);

        globalVertexOffset = mVertices.size() / floatsPerVertex;
        globalIndexOffset = mIndices.size();
        globalTransformOffset = mTransforms.size() / floatsPerTransform;
    }
}

const std::unordered_map<std::string, MyModel>& ModelHandler::getModels()
{
    return mModels;
}

const std::vector<float>& ModelHandler::getVertices()
{
    return mVertices;
}

const std::vector<uint32_t>& ModelHandler::getIndices()
{
    return mIndices;
}

const std::vector<float>& ModelHandler::getTransforms()
{
    return mTransforms;
}

std::vector<float> ModelHandler::generateBoundingBox(glm::vec3 min, glm::vec3 max)
{
    return std::vector
    {
        // top
        min.x, min.y, max.z, max.x, min.y, max.z,
        min.x, max.y, max.z, max.x, max.y, max.z,
        min.x, min.y, max.z, min.x, max.y, max.z,
        max.x, min.y, max.z, max.x, max.y, max.z,

        // bottom
        min.x, min.y, min.z, max.x, min.y, min.z,
        min.x, max.y, min.z, max.x, max.y, min.z,
        min.x, min.y, min.z, min.x, max.y, min.z,
        max.x, min.y, min.z, max.x, max.y, min.z,

        // sides
        min.x, min.y, min.z, min.x, min.y, max.z,
        max.x, min.y, min.z, max.x, min.y, max.z,
        max.x, max.y, min.z, max.x, max.y, max.z,
        min.x, max.y, min.z, min.x, max.y, max.z,
    };
}
*/
