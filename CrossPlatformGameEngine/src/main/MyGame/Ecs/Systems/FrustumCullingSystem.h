#pragma once

#include <map>

#include "../../../EcsSystem/SystemManager/System/System_I.h"

#include "../../../Engine/Core/Engine.h"
#include "../../ModelHandler/ModelHandler.h"

#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class FrustumCullingSystem : public System_I
{
public:
    struct alignas(16) Entity
    {
        glm::vec3 mPosition;
        uint32_t _pad0;
        glm::vec3 mRotation;
        uint32_t _pad1;
        glm::vec3 mScale;
        uint32_t _pad2;

        glm::vec3 mBoundsPos;
        uint32_t _pad3;
        glm::vec3 mBoundsNeg;

        uint32_t mIsVisible;

        uint32_t mMeshCount;
        uint32_t mMeshOffset;
        uint32_t _pad4[2];
    };

public:
    FrustumCullingSystem(ascen::Engine& engine, const std::unordered_map<std::string, MyModel>& models);

    void update(const float delta) override;

private:
    ascen::Engine& mEngine;
    const std::unordered_map<std::string, MyModel>& mModels;

    std::map<uint32_t, ascen::IndexedIndirectDraw> mModelToDrawCommands;
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, Entity>> mModelToEntities;

    std::vector<ascen::IndexedIndirectDraw> mDrawCommands;
    std::vector<Entity> mEntitiesToCull;
};
