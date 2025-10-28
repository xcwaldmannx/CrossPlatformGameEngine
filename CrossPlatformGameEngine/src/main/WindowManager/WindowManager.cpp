#include "WindowManager.h"

#include <stdexcept>

WindowManager::WindowManager()
{

}

void WindowManager::init()
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    mWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan Window", nullptr, nullptr);
    if (!mWindow)
        throw std::runtime_error("Failed to create GLFW window");
}

void WindowManager::destroy()
{
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void WindowManager::pollEvents() const
{
    glfwPollEvents();
}

bool WindowManager::isRunning() const
{
    return !glfwWindowShouldClose(mWindow);
}

GLFWwindow* WindowManager::getWindow() const
{
	return mWindow;
}