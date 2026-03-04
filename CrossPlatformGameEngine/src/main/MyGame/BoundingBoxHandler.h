#pragma once

#include "../Engine/Core/Engine.h"

struct BBoxInput
{
    int x;
    int y;
};

struct BBoxOutput
{
    int sum;
};


class BoundingBoxHandler
{
public:
    BoundingBoxHandler(ascen::Engine& engine);

    void update();

    void print();

private:
    ascen::Engine& mEngine;

    std::vector<BBoxInput> mInputs;
    std::vector<BBoxOutput> mOutputs;
};