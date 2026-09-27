#pragma once

#include "../../../EcsSystem/SystemManager/System/System_I.h"
#include "../../../Engine/Core/Engine.h"

#include <Mal.h>

struct AnimationData
{
    int32_t mBoneTransformOffset = -1;
};

class AnimationSystem : public System_I
{
public:
    AnimationSystem(ascen::Engine& engine, std::unordered_map<std::string, mal::AnimationPlayer>& players);

    void update(const float delta) override;

private:
    ascen::Engine& mEngine;
    std::unordered_map<std::string, mal::AnimationPlayer>& mPlayers;
};