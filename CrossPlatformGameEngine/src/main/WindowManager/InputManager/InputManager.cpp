#include "InputManager.h"

#include <iostream>
#include <ostream>

std::array<std::atomic<int>, InputManager::MAX_KEYS> InputManager::mCurrKeys{};
std::array<std::atomic<int>, InputManager::MAX_KEYS> InputManager::mPrevKeys{};

std::array<std::atomic<int>, InputManager::MAX_MOUSE_BUTTONS> InputManager::mCurrButtons{};
std::array<std::atomic<int>, InputManager::MAX_MOUSE_BUTTONS> InputManager::mPrevButtons{};

std::atomic<bool> InputManager::mFirstMouse { true };
std::atomic<bool> InputManager::mLockMouseToCenter { true };
std::atomic<double> InputManager::mMouseX{  0.0 };
std::atomic<double> InputManager::mMouseY { 0.0 };
std::atomic<double> InputManager::mMouseDeltaX { 0.0 };
std::atomic<double> InputManager::mMouseDeltaY { 0.0 };

InputManager::InputManager() {}

void InputManager::update(GLFWwindow* window)
{
    for (int i = 0; i < MAX_KEYS; ++i)
    {
        mPrevKeys[i].store(mCurrKeys[i].load(std::memory_order_relaxed), std::memory_order_relaxed);
    }

    for (int i = 0; i < MAX_MOUSE_BUTTONS; ++i)
    {
        mPrevButtons[i].store(mCurrButtons[i].load(std::memory_order_relaxed), std::memory_order_relaxed);
    }

    for (int i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST; ++i)
    {
        mCurrKeys[i].store(glfwGetKey(window, i) == GLFW_PRESS ? 1 : 0, std::memory_order_relaxed);
    }

    for (int i = 0; i < MAX_MOUSE_BUTTONS; ++i)
    {
        mCurrButtons[i].store(glfwGetMouseButton(window, i) == GLFW_PRESS ? 1 : 0, std::memory_order_relaxed);
    }
}

void InputManager::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (mFirstMouse.load(std::memory_order_relaxed))
    {
        mMouseX.store(xpos, std::memory_order_relaxed);
        mMouseY.store(ypos, std::memory_order_relaxed);

        mMouseDeltaX.store(0, std::memory_order_relaxed);
        mMouseDeltaY.store(0, std::memory_order_relaxed);

        mFirstMouse.store(false, std::memory_order_relaxed);
    }
    else
    {
        const double oldMouseX = mMouseX.exchange(xpos, std::memory_order_relaxed);
        const double oldMouseY = mMouseY.exchange(ypos, std::memory_order_relaxed);

        mMouseDeltaX.fetch_add(xpos - oldMouseX, std::memory_order_relaxed);
        mMouseDeltaY.fetch_add(ypos - oldMouseY, std::memory_order_relaxed);
    }
}

bool InputManager::isKeyPressed(int key)
{
    if (key < 0 || key >= MAX_KEYS)
    {
        return false;
    }

    return mCurrKeys[key].load(std::memory_order_relaxed) == GLFW_PRESS;
}

bool InputManager::isKeyJustPressed(int key)
{
    if (key < 0 || key >= MAX_KEYS)
    {
        return false;
    }

    return mCurrKeys[key].load(std::memory_order_relaxed) == GLFW_PRESS &&
        mPrevKeys[key].load(std::memory_order_relaxed) == GLFW_RELEASE;
}

bool InputManager::isKeyJustReleased(int key)
{
    if (key < 0 || key >= MAX_KEYS)
    {
        return false;
    }

    return mCurrKeys[key].load(std::memory_order_relaxed) == GLFW_RELEASE &&
        mPrevKeys[key].load(std::memory_order_relaxed) == GLFW_PRESS;
}

bool InputManager::isButtonPressed(int button)
{
    if (button < 0 || button >= MAX_MOUSE_BUTTONS)
    {
        return false;
    }

    return mCurrButtons[button].load(std::memory_order_relaxed) == GLFW_PRESS;
}

bool InputManager::isButtonJustPressed(int button)
{
    if (button < 0 || button >= MAX_MOUSE_BUTTONS)
    {
        return false;
    }

    return mCurrButtons[button].load(std::memory_order_relaxed) == GLFW_PRESS &&
        mPrevButtons[button].load(std::memory_order_relaxed) == GLFW_RELEASE;
}

bool InputManager::isButtonJustReleased(int button)
{
    if (button < 0 || button >= MAX_MOUSE_BUTTONS)
    {
        return false;
    }

    return mCurrButtons[button].load(std::memory_order_relaxed) == GLFW_RELEASE &&
        mPrevButtons[button].load(std::memory_order_relaxed) == GLFW_PRESS;
}

double InputManager::getMouseX()
{
    return mMouseX.load(std::memory_order_relaxed);
}

double InputManager::getMouseY()
{
    return mMouseY.load(std::memory_order_relaxed);
}

double InputManager::getMouseDeltaX()
{ 
    return mMouseDeltaX.exchange(0, std::memory_order_relaxed);
}

double InputManager::getMouseDeltaY()
{ 
    return mMouseDeltaY.exchange(0, std::memory_order_relaxed);
}
