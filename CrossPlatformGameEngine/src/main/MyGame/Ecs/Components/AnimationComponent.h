#pragma once

#include <random>

#include <string>

struct AnimationComponent
{
    AnimationComponent()
    {
        const float r = rand() % 15;

        // if (r < 5.0f)
        // {
        //     mCurrentAnimation = "dead";
        // }
        // else if (r >= 5.0f && r < 10.0f)
        // {
        //     mCurrentAnimation = "idle";
        // }
        // else
        // {
        //     mCurrentAnimation = "attack";
        // }

        mCurrentTime = rand() % 2048;
    }

    std::string mCurrentAnimation;
    float mCurrentTime;
};
