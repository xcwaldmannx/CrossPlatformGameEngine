#pragma once

#include <atomic>
#include <array>

#include <GLFW/glfw3.h>

class InputManager
{
public:
	InputManager();

	static void update(GLFWwindow* window);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

    static bool isKeyPressed(int key);
    static bool isKeyJustPressed(int key);
    static bool isKeyJustReleased(int key);
    static bool isButtonPressed(int button);
    static bool isButtonJustPressed(int button);
    static bool isButtonJustReleased(int button);

    static double getMouseX();
    static double getMouseY();
    static double getMouseDeltaX();
    static double getMouseDeltaY();

private:
    static constexpr int MAX_KEYS = GLFW_KEY_LAST + 1;
    static constexpr int MAX_MOUSE_BUTTONS = GLFW_MOUSE_BUTTON_LAST + 1;

    static std::array<std::atomic<int>, MAX_KEYS> mCurrKeys;
    static std::array<std::atomic<int>, MAX_KEYS> mPrevKeys;

    static std::array<std::atomic<int>, MAX_MOUSE_BUTTONS> mCurrButtons;
    static std::array<std::atomic<int>, MAX_MOUSE_BUTTONS> mPrevButtons;

    static std::atomic<bool> mFirstMouse;
    static std::atomic<bool> mLockMouseToCenter;
    static std::atomic<double> mMouseX;
    static std::atomic<double> mMouseY;
    static std::atomic<double> mMouseDeltaX;
    static std::atomic<double> mMouseDeltaY;
};
