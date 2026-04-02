#pragma once

#include <Mass.h>

struct MyModel
{
    uint32_t mModelId = 0;
    uint32_t mVertexOffset    = 0;
    uint32_t mIndexOffset     = 0;
    uint32_t mIndexCount      = 0;
    uint32_t mTransformOffset = 0;
    uint32_t mBoundsOffset    = 0;
};

class ModelHandler
{
public:
    ModelHandler();

    void loadModels(const std::vector<std::string>& filepaths);

    const std::unordered_map<std::string, MyModel>& getModels();
    const std::vector<float>& getVertices();
    const std::vector<uint32_t>& getIndices();
    const std::vector<float>& getTransforms();
    const std::vector<float>& getBounds();

private:
    std::vector<float> generateBoundingBox(glm::vec3 min, glm::vec3 max);

private:
    std::unordered_map<std::string, MyModel> mModels;

    std::vector<float> mVertices;
    std::vector<uint32_t> mIndices;
    std::vector<float> mTransforms;
    std::vector<float> mBounds;
};