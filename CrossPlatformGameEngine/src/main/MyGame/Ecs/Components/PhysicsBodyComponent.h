#pragma once

#include <box3d/box3d.h>

enum PhysicsBodyType
{
    DYNAMIC
};

struct PhysicsBodyComponent
{
    PhysicsBodyType mBodyType = DYNAMIC;
    float mDensity = 1;
};