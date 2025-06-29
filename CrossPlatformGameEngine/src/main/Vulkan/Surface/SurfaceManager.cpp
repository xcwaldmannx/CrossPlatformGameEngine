#include "SurfaceManager.h"

#include <stdexcept>

using namespace vulkan;

class GLFWwindow;

SurfaceManager::SurfaceManager() {

}

VkResult SurfaceManager::create(const VkInstance& instance, GLFWwindow* window) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &mVkSurface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

    return VK_SUCCESS;
}

void SurfaceManager::destroy(const VkInstance& instance) {
    vkDestroySurfaceKHR(instance, mVkSurface, nullptr);
}

const VkSurfaceKHR& SurfaceManager::getSurface() const {
    return mVkSurface;
}
