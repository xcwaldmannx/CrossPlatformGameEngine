#include "WindowManager.h"

#include <iostream>
#include <stdexcept>

void WindowManager::create()
{
    mWindowThread = std::thread(&WindowManager::windowThread, this);

    while (!mWindowReady.load(std::memory_order_acquire))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

}

void WindowManager::destroy()
{
    mIsRunning.store(false, std::memory_order_release);

    if (mWindowThread.joinable())
    {
        mWindowThread.join();
    }

    mIsRunning.store(false, std::memory_order_release);
    mWindowReady.store(false, std::memory_order_release);

    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void WindowManager::poll()
{
    mPoll.store(true, std::memory_order_release);
}

void WindowManager::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    mIsResized.store(true, std::memory_order_release);
    mFramebufferWidth.store(width, std::memory_order_release);
    mFramebufferHeight.store(height, std::memory_order_release);
}

void WindowManager::iconifyCallback(GLFWwindow* window, int iconified)
{
    // iconified == 1, minimized  
    // iconified == 0, restored
    mIsMinimized.store(iconified == GLFW_TRUE, std::memory_order_release);
}

void WindowManager::windowCloseCallback(GLFWwindow* window)
{
    mIsCloseRequested.store(true, std::memory_order_release);
}

void WindowManager::setResized(bool resized)
{
    mIsResized.store(resized, std::memory_order_release);
}

bool WindowManager::isRunning()
{
    return mIsRunning.load(std::memory_order_acquire);
}

bool WindowManager::isResized()
{
    return mIsResized.load(std::memory_order_acquire);
}

bool WindowManager::isMinimized()
{
    return mIsMinimized.load(std::memory_order_acquire);
}

bool WindowManager::isCloseRequested()
{
    return mIsCloseRequested.load(std::memory_order_acquire);
}

bool WindowManager::isMouseFocused()
{
    return mFocusMouse.load(std::memory_order_acquire);
}

GLFWwindow* WindowManager::getWindow() const
{
	return mWindow;
}

InputManager& WindowManager::getInput()
{
    return mInputManager;
}

void WindowManager::windowThread()
{
    glfwSetErrorCallback(
    [](int error, const char* description)
    {
        std::cerr
            << "GLFW error " << error << ": "
            << (description ? description : "unknown")
            << '\n';
    });

    // enable this for renderdoc
    // glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    mWindow = glfwCreateWindow(
        static_cast<int>(WINDOW_WIDTH),
        static_cast<int>(WINDOW_HEIGHT),
        "Vulkan Window",
        nullptr,
        nullptr);

    if (!mWindow)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    mFramebufferWidth.store(WINDOW_WIDTH, std::memory_order_release);
    mFramebufferHeight.store(WINDOW_HEIGHT, std::memory_order_release);

    glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);
    glfwSetWindowIconifyCallback(mWindow, iconifyCallback);
    glfwSetWindowCloseCallback(mWindow, windowCloseCallback);
    glfwSetCursorPosCallback(mWindow, InputManager::mouseCallback);

    mIsRunning.store(true, std::memory_order_release);
    mWindowReady.store(true, std::memory_order_release);

    glfwMakeContextCurrent(mWindow);
    glfwSwapInterval(1);

    while (!mIsCloseRequested.load(std::memory_order_acquire) && mIsRunning.load(std::memory_order_acquire))
    {
        const auto& poll = mPoll.load(std::memory_order_acquire);
        if (poll)
        {
            glfwPollEvents();
            InputManager::update(mWindow);

            if (InputManager::isKeyJustPressed(GLFW_KEY_TAB))
            {
                const bool focus = !mFocusMouse.load(std::memory_order_acquire);

                mFocusMouse.exchange(focus, std::memory_order_release);
                glfwFocusWindow(mWindow);

                if (!focus)
                {
                    glfwSetCursorPos(mWindow, getCenterX(), getCenterY());
                }

                const int cursorMode = focus ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
                glfwSetInputMode(mWindow, GLFW_CURSOR, cursorMode);
            }

            mPoll.store(false, std::memory_order_release);
        }
    }
}
