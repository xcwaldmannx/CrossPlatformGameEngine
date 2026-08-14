#include "Surface.h"

#include <iostream>
#include <stdexcept>

using namespace ascen;

VkSurfaceKHR Surface::create(VkInstance instance, GLFWwindow* window)
{
    VkSurfaceKHR surface;

    const VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);

    if (result != VK_SUCCESS)
    {
        const char* description = nullptr;
        const int error = glfwGetError(&description);

        std::cerr
            << "GLFW error " << error << ": "
            << (description ? description : "no description")
            << '\n';

        throw std::runtime_error(
            "glfwCreateWindowSurface failed with VkResult " +
            std::to_string(static_cast<int>(result)));

    }

    return surface;
}

void Surface::destroy(VkInstance instance, VkSurfaceKHR surface)
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}
