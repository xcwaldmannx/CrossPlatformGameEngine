#include "WindowManager.h"

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
}

void WindowManager::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    mIsResized.store(true, std::memory_order_release);
}

void WindowManager::setResized(bool resized)
{
    mIsResized.store(resized, std::memory_order_release);
}

bool WindowManager::isRunning() const
{
    return mIsRunning.load(std::memory_order_acquire);
}

bool WindowManager::isResized() const
{
    return mIsResized.load(std::memory_order_acquire);
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
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    mWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan Window", nullptr, nullptr);
    if (!mWindow)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    mIsRunning.store(true, std::memory_order_release);
    mWindowReady.store(true, std::memory_order_release);

    glfwMakeContextCurrent(mWindow);
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(mWindow) && mIsRunning.load(std::memory_order_acquire))
    {
        glfwPollEvents();
        mInputManager.update(mWindow);
    }

    mIsRunning.store(false, std::memory_order_release);
    mWindowReady.store(false, std::memory_order_release);

    glfwDestroyWindow(mWindow);
    glfwTerminate();
}
