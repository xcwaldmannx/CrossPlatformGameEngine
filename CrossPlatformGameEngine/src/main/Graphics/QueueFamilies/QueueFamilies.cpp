#include "QueueFamilies.h"

#include <vector>

using namespace ascen;

void QueueFamilies::updateQueueFamilies(
	VkPhysicalDevice physicalDevice,
	VkSurfaceKHR surface,
	QueueFamily& graphicsFamily,
	QueueFamily& presentFamily)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

        if (presentSupport)
        {
            presentFamily = i;
        }

        if (graphicsFamily.has_value() && presentFamily.has_value())
        {
            break;
        }

        i++;
    }
}

VkQueue QueueFamilies::getDeviceQueue(VkDevice device, uint32_t index)
{
    VkQueue queue;
    vkGetDeviceQueue(device, index, 0, &queue);
    return queue;
}
