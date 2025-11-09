#pragma once

#include <chrono>
#include <thread>

class FrameCounter
{
public:
    FrameCounter(unsigned int targetFPS = 60)
        : mTargetFPS(targetFPS),
        mFrameCount(0),
        mFPS(0),
        mDeltaTime(0.0f)
    {
        mLastTime = std::chrono::high_resolution_clock::now();
        mLastFPSTime = mLastTime;
        mFrameDuration = std::chrono::duration<double>(1.0 / mTargetFPS);
    }

    void frame()
    {
        using namespace std::chrono;
        auto now = high_resolution_clock::now();
        duration<double> elapsed = now - mLastTime;

        // Frame rate limiting with high-precision sleep
        if (elapsed < mFrameDuration)
        {
            auto remaining = mFrameDuration - elapsed;

            // Sleep until ~1ms before target
            if (remaining > std::chrono::milliseconds(1))
                std::this_thread::sleep_for(remaining - std::chrono::milliseconds(1));

            // Busy wait the last microseconds for precision
            while (high_resolution_clock::now() - mLastTime < mFrameDuration)
                std::this_thread::yield();

            now = high_resolution_clock::now();
            elapsed = now - mLastTime;
        }

        mDeltaTime = static_cast<float>(elapsed.count());
        mLastTime = now;

        ++mFrameCount;

        // Update FPS every second
        duration<double> fpsElapsed = now - mLastFPSTime;
        if (fpsElapsed.count() >= 1.0)
        {
            mFPS = mFrameCount;
            mFrameCount = 0;
            mLastFPSTime = now;
        }
    }

    float deltaTime() const { return mDeltaTime; }
    unsigned int fps() const { return mFPS; }
    void setTargetFPS(unsigned int fps)
    {
        mTargetFPS = fps;
        mFrameDuration = std::chrono::duration<double>(1.0 / mTargetFPS);
    }

private:
    std::chrono::high_resolution_clock::time_point mLastTime;
    std::chrono::high_resolution_clock::time_point mLastFPSTime;
    std::chrono::duration<double> mFrameDuration;

    unsigned int mTargetFPS;
    unsigned int mFrameCount;
    unsigned int mFPS;
    float mDeltaTime;
};
