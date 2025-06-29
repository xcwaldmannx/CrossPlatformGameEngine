#include "WindowManager.h"

#include <stdexcept>

using namespace vulkan;

void WindowManager::create() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Disable OpenGL if using Vulkan
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);    // Allow window resizing

    mGlfwWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My GLFW Window", nullptr, nullptr);
    if (!mGlfwWindow) {
        throw std::runtime_error("Failed to create GLFW window.");
        glfwTerminate();
    }

    glfwSetWindowUserPointer(mGlfwWindow, this);
    glfwSetFramebufferSizeCallback(mGlfwWindow, frameBufferResizeCallback);
}

void WindowManager::destroy() {
    glfwDestroyWindow(mGlfwWindow);
    glfwTerminate();
}

GLFWwindow* WindowManager::getWindow() const {
    return mGlfwWindow;
}

bool WindowManager::isResized() {
    return mIsResized;
}

void WindowManager::frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto thisWindow = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    thisWindow->mIsResized;

}