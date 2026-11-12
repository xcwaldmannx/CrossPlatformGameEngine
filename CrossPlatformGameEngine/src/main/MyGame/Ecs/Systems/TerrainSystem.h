#pragma once

#include "../../../EcsSystem/SystemManager/System/System_I.h"
#include "../../../Engine/Core/Engine.h"

#include <glm/glm.hpp>

class TerrainSystem : public System_I
{
public:
    TerrainSystem(ascen::Engine& engine);

    void update(const float delta) override;

private:
    ascen::Engine& mEngine;
    glm::ivec2 mLastChunk;
};