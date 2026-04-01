#pragma once

#include "../../../EcsSystem/SystemManager/System/System_I.h"

#include "../../../Engine/Core/Engine.h"
#include "../../ModelHandler/ModelHandler.h"

#include <glm/glm.hpp>

class SimpleRenderSystem : public System_I
{
public:
    struct Model
    {
        uint32_t mModelOffset;
    };

public:
    SimpleRenderSystem(ascen::Engine& engine, const std::unordered_map<std::string, MyModel>& models);

    void update(float deltaTime) override;

private:
    ascen::Engine& mEngine;
    const std::unordered_map<std::string, MyModel>& mModels;
};
