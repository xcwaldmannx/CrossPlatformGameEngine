#pragma once

#include <unordered_map>

#include <Mal.h>

struct Model
{
    mal::model::Model mModel;
    mal::anim::AnimationSet mAnimationSet;

    uint32_t mId = 0;
    uint32_t mVertexOffset = 0;
    uint32_t mIndexOffset = 0;
    uint32_t mIndexCount = 0;
    uint32_t mTextureOffset = 0;
    uint32_t mMaterialOffset = 0;
    glm::vec3 mBoundsMax = glm::vec3(0.0f);
    glm::vec3 mBoundsMin = glm::vec3(0.0f);
};

struct Material
{
    glm::vec4 mBaseColor{ 1.0f };
    float mMetallic = 1.0f;
    float mRoughness = 1.0f;

    int32_t mBaseColorTextureId = -1;
    int32_t mNormalTextureId = -1;
    int32_t mMetallicRoughnessTextureId = -1;
    int32_t mOcclusionTextureId = -1;
    int32_t mEmissiveTextureId = -1;
    int32_t _pad;
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

    const mal::model::VertexArray& getVertices();
    const mal::model::IndexArray& getIndices();
    const mal::model::TextureArray& getTextures();
    const mal::model::MaterialArray& getMaterials();

private:
    std::unordered_map<std::string, Model> mModels;
    std::unordered_map<std::string, mal::AnimationPlayer> mPlayers;

    inline static uint32_t sGlobalVertexOffset = 0;
    inline static uint32_t sGlobalIndexOffset = 0;
    inline static uint32_t sGlobalTextureOffset = 0;
    inline static uint32_t sGlobalMaterialOffset = 0;

    mal::model::VertexArray mVertices;
    mal::model::IndexArray mIndices;
    mal::model::TextureArray mTextures;
    mal::model::MaterialArray mMaterials;
};
