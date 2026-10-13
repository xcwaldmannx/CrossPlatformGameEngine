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

    mal::AnimationPlayer& getPlayer(const std::string& name);
    std::unordered_map<std::string, mal::AnimationPlayer>& getPlayers();

    const std::vector<mal::model::Vertex>& getVertices();
    const std::vector<uint32_t>& getIndices();

private:
    std::unordered_map<std::string, Model> mModels;
    std::unordered_map<std::string, mal::AnimationPlayer> mPlayers;

    inline static uint32_t sGlobalVertexOffset = 0;
    inline static uint32_t sGlobalIndexOffset = 0;

    std::vector<mal::model::Vertex> mVertices;
    std::vector<uint32_t> mIndices;
};
