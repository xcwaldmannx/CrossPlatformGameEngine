#pragma once

#include <unordered_map>

#include <Mal.h>
#include <glm/vec3.hpp>

struct Model
{
    mal::model::Model mModel;
    mal::anim::AnimationSet mAnimationSet;

    uint32_t mId = 0;
    uint32_t mVertexOffset = 0;
    uint32_t mIndexOffset = 0;
    uint32_t mIndexCount = 0;
    glm::vec3 mBoundsMax = glm::vec3(0.0f);
    glm::vec3 mBoundsMin = glm::vec3(0.0f);
};

class ModelHandler
{
public:
    ModelHandler();

    void load(const std::string& name, const std::string& filename);

    const Model& getModel(const std::string& name);
    const std::unordered_map<std::string, Model>& getModels();

    const std::vector<mal::model::Vertex>& getVertices();
    const std::vector<uint32_t>& getIndices();

private:
    std::unordered_map<std::string, Model> mModels;

    inline static uint32_t mGlobalVertexOffset = 0;
    inline static uint32_t mGlobalIndexOffset = 0;

    std::vector<mal::model::Vertex> mVertices;
    std::vector<uint32_t> mIndices;
};

/*
#include <Mass.h>

struct MyModel
{
    uint32_t mModelId = 0;
    uint32_t mVertexOffset    = 0;
    uint32_t mIndexOffset     = 0;
    uint32_t mIndexCount      = 0;
    uint32_t mTransformOffset = 0;
    glm::vec3 mBoundsPos;
    glm::vec3 mBoundsNeg;
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

private:
    std::vector<float> generateBoundingBox(glm::vec3 min, glm::vec3 max);

private:
    std::unordered_map<std::string, MyModel> mModels;

    std::vector<float> mVertices;
    std::vector<uint32_t> mIndices;
    std::vector<float> mTransforms;
};
*/
