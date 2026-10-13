#pragma once

#include <random>

#include <string>

struct AnimationComponent
{
    AnimationComponent()
    {
        mCurrentTime = rand() % 2048;
    }

    std::string mCurrentAnimation;
    float mCurrentTime;
};
