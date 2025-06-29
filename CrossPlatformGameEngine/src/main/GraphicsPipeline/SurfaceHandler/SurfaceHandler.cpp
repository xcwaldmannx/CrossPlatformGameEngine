#include "SurfaceHandler.h"

#include <stdexcept>

class GLFWwindow;

/*
Begin SurfaceHandler Implementation
*/

namespace ascen {

    void createSurface(Instance& instance, GLFWwindow* window, Surface* surface) {
        if (glfwCreateWindowSurface(instance.mInstance, window, nullptr, &surface->mSurface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void destroySurface(Instance& instance, Surface& surface) {
        vkDestroySurfaceKHR(instance.mInstance, surface.mSurface, nullptr);
    }

}

/*
End SurfaceHandler Implementation
*/