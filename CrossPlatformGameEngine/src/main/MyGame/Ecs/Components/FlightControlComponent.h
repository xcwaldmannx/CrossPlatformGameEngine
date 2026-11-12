#pragma once

#include <variant>

struct Helicopter
{
    float mLift = 0.0f;
    float mPitchRate = 0.0f;
    float mRollRate = 0.0f;
};

struct Huey : Helicopter
{
    Huey()
    {
        mLift = 0.0f;
        mPitchRate = 0.0f;
        mRollRate = 0.0f;
    }
};

struct Chinook : Helicopter
{
    Chinook()
    {
        mLift = 9.81f;
        mPitchRate = 0.0f;
        mRollRate = 0.0f;
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