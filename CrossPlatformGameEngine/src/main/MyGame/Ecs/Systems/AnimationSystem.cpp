#include "AnimationSystem.h"

#include "../../../EcsSystem/EcsSystem.h"

#include "../Components/AnimationComponent.h"
#include "../Components/ModelComponent.h"

AnimationSystem::AnimationSystem(ascen::Engine& engine, std::unordered_map<std::string, mal::AnimationPlayer>& players) :
    mEngine(engine), mPlayers(players) {}

void AnimationSystem::update(const float delta)
{
    std::vector<AnimationData> animationData;
    animationData.resize(ENTITY_MAX);
    std::vector<glm::mat4> boneTransforms;

    for (auto& e : mEntities)
    {
        const auto& model = mSystem->getComponent<ModelComponent>(e);
        auto& anim = mSystem->getComponent<AnimationComponent>(e);

        auto& player = mPlayers.at(model.mName);
        player.setDefaultScene();
        player.play(anim.mCurrentAnimation, anim.mCurrentTime);
        player.update(delta);
        anim.mCurrentTime = player.getCurrentTime();

        animationData.at(e).mBoneTransformOffset = static_cast<int32_t>(boneTransforms.size());
        boneTransforms.append_range(player.getBoneTransforms());
    }

    if (!boneTransforms.empty())
    {
        mEngine.uploadBuffer("BUFFER_ANIM_DATA", &animationData[0], animationData.size(), sizeof(AnimationData), 0);
        mEngine.uploadBuffer("BUFFER_BONE_TRANS", &boneTransforms[0], boneTransforms.size(), sizeof(glm::mat4), 0);
    }
}
