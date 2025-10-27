#include "QueueFamilyHandler.h"

#include <vector>

/*
Begin QueueFamilyHandler Implementation
*/

namespace ascen {

    void updateQueueFamilies(
        PhysicalDevice& physicalDevice,
        Surface& surface,
        std::optional<uint32_t>& graphicsFamily,
        std::optional<uint32_t>& presentFamily) {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice.mDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice.mDevice, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice.mDevice, i, surface.mSurface, &presentSupport);

            if (presentSupport) {
                presentFamily = i;
            }

            if (graphicsFamily.has_value() && presentFamily.has_value()) {
                break;
            }

            i++;
        }
    }

    VkQueue getDeviceQueue(LogicalDevice& logicalDevice, uint32_t index) {
        VkQueue queue;
        vkGetDeviceQueue(logicalDevice.mDevice, index, 0, &queue);
        return queue;
    }

}

/*
End QueueFamilyHandler Implementation
*/