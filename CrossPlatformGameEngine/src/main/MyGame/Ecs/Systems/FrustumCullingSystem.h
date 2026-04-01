#pragma once

#include <map>

#include "../../../EcsSystem/SystemManager/System/System_I.h"

#include "../../../Engine/Resource/Buffer/Indirect/IndirectBuffer.h"

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
        uint32_t mIsVisible;

        uint32_t mMeshCount;
        uint32_t mMeshOffset;
        uint32_t _pad2[2];
    };

    struct alignas(16) Mesh
    {
        uint32_t mVertexOffset;

        bool operator< (const Mesh& other) const
        {
            return mVertexOffset < other.mVertexOffset;
        }
    };

public:
    FrustumCullingSystem(ascen::Engine& engine, const std::unordered_map<std::string, MyModel>& models);

    void update(float deltaTime) override;

private:
    ascen::Engine& mEngine;
    const std::unordered_map<std::string, MyModel>& mModels;

    std::map<uint32_t, ascen::IndirectBuffer::IndexedIndirectCommand> mModelToDrawCommands;
    std::unordered_map<uint32_t, std::vector<Entity>> mModelToEntities;

    std::vector<ascen::IndirectBuffer::IndexedIndirectCommand> mDrawCommands;
    std::vector<Entity> mEntitiesToCull;
};
