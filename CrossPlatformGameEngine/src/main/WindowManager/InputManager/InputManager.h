#pragma once

#include <atomic>
#include <array>

#include <GLFW/glfw3.h>

class InputManager
{
public:
	InputManager();

	void update(GLFWwindow* window);

    bool isKeyPressed(int key) const;
    bool isKeyJustPressed(int key) const;
    bool isKeyJustReleased(int key) const;
    bool isButtonPressed(int button) const;
    bool isButtonJustPressed(int button) const;
    bool isButtonJustReleased(int button) const;

    double getMouseX() const;
    double getMouseY() const;
    double getMouseDeltaX() const;
    double getMouseDeltaY() const;

private:
    static const int MAX_KEYS = GLFW_KEY_LAST + 1;
    static const int MAX_MOUSE_BUTTONS = GLFW_MOUSE_BUTTON_LAST + 1;

    std::array<std::atomic<int>, MAX_KEYS> mCurrKeys{};
    std::array<std::atomic<int>, MAX_KEYS> mPrevKeys{};

    std::array<std::atomic<int>, MAX_MOUSE_BUTTONS> mCurrButtons{};
    std::array<std::atomic<int>, MAX_MOUSE_BUTTONS> mPrevButtons{};

    std::atomic<double> mMouseX = 0.0;
    std::atomic<double> mMouseY = 0.0;
    std::atomic<double> mMouseDeltaX = 0.0;
    std::atomic<double> mMouseDeltaY = 0.0;
};
