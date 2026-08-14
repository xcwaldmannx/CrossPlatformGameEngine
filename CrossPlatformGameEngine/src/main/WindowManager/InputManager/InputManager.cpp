#include "InputManager.h"

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

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    mMouseDeltaX.store(mouseX - mMouseX.load(std::memory_order_relaxed), std::memory_order_relaxed);
    mMouseDeltaY.store(mouseY - mMouseY.load(std::memory_order_relaxed), std::memory_order_relaxed);

    mMouseX.store(mouseX, std::memory_order_relaxed);
    mMouseY.store(mouseY, std::memory_order_relaxed);
}

bool InputManager::isKeyPressed(int key) const
{
    if (key < 0 || key >= MAX_KEYS)
    {
        return false;
    }

    return mCurrKeys[key].load(std::memory_order_relaxed) == GLFW_PRESS;
}

bool InputManager::isKeyJustPressed(int key) const
{
    if (key < 0 || key >= MAX_KEYS)
    {
        return false;
    }

    return mCurrKeys[key].load(std::memory_order_relaxed) == GLFW_PRESS &&
        mPrevKeys[key].load(std::memory_order_relaxed) == GLFW_RELEASE;
}

bool InputManager::isKeyJustReleased(int key) const
{
    if (key < 0 || key >= MAX_KEYS)
    {
        return false;
    }

    return mCurrKeys[key].load(std::memory_order_relaxed) == GLFW_RELEASE &&
        mPrevKeys[key].load(std::memory_order_relaxed) == GLFW_PRESS;
}

bool InputManager::isButtonPressed(int button) const
{
    if (button < 0 || button >= MAX_MOUSE_BUTTONS)
    {
        return false;
    }

    return mCurrButtons[button].load(std::memory_order_relaxed) == GLFW_PRESS;
}

bool InputManager::isButtonJustPressed(int button) const
{
    if (button < 0 || button >= MAX_MOUSE_BUTTONS)
    {
        return false;
    }

    return mCurrButtons[button].load(std::memory_order_relaxed) == GLFW_PRESS &&
        mPrevButtons[button].load(std::memory_order_relaxed) == GLFW_RELEASE;
}

bool InputManager::isButtonJustReleased(int button) const
{
    if (button < 0 || button >= MAX_MOUSE_BUTTONS)
    {
        return false;
    }

    return mCurrButtons[button].load(std::memory_order_relaxed) == GLFW_RELEASE &&
        mPrevButtons[button].load(std::memory_order_relaxed) == GLFW_PRESS;
}

double InputManager::getMouseX() const
{
    return mMouseX.load(std::memory_order_relaxed);
}

double InputManager::getMouseY() const
{
    return mMouseY.load(std::memory_order_relaxed);
}

double InputManager::getMouseDeltaX() const
{ 
    return mMouseDeltaX.load(std::memory_order_relaxed);
}

double InputManager::getMouseDeltaY() const
{ 
    return mMouseDeltaY.load(std::memory_order_relaxed);
}
