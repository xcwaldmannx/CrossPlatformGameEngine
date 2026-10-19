#pragma once

#include <variant>

struct Helicopter
{
    float mAccelYaw = 0;
    float mAccelPitch = 0;
    float mAccelRoll = 0;
    float mLift = 0;
};

struct Huey : Helicopter
{
    Huey()
    {
        mAccelYaw = 0;
        mAccelPitch = 0;
        mAccelRoll = 0;
        mLift = 0;
    }
};

struct Chinook : Helicopter
{
    Chinook()
    {
        mAccelYaw = 0;
        mAccelPitch = 0;
        mAccelRoll = 0;
        mLift = 0;
    }
};

using Aircraft = std::variant<Huey, Chinook>;

enum class AircraftType
{
    HUEY,
    CHINOOK,
};

struct FlightControlComponent
{
    AircraftType mType;
    Aircraft mAircraft;
    bool mIsControlled = false;
};