#pragma once

#include "../../Engine/Core/Engine.h"

class MyPipeline
{
public:
    MyPipeline(ascen::Engine& engine);

    void init();

private:
    void initResources();
    void initStages();
    void initFramePasses();

private:
    ascen::Engine& mEngine;
};