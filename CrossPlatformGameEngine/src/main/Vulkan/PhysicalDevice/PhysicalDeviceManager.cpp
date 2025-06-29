#include "PhysicalDeviceManager.h"

#include <stdexcept>

#include <optional>
#include <set>
#include <string>

using namespace vulkan;

void PhysicalDeviceManager::establishPhysicalDevice(const VkInstance& instance,
    const VkSurfaceKHR& surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    

    for (const auto& device : devices) {
        if (checkDeviceSupport(device, surface)) {
            mVkPhysicalDevice = device;
            break;
        }
    }

    if (mVkPhysicalDevice == nullptr) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

bool PhysicalDeviceManager::checkDeviceSupport(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) {
    // QueueFamily Support
    bool hasQueueFamilySupport = false;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    std::optional<uint32_t> graphicsFamilyIndex;
    std::optional<uint32_t> presentFamilyIndex;

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamilyIndex = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

        if (presentSupport) {
            presentFamilyIndex = i;
        }

        if (graphicsFamilyIndex.has_value() && presentFamilyIndex.has_value()) {
            hasQueueFamilySupport = true;
            break;
        }

        i++;
    }

    // Surface Support
    bool hasSurfaceSupport = false;

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    hasSurfaceSupport = (formatCount != 0 && presentModeCount != 0) ? true : false;

    // Extension Support
    bool hasExtensionSupport = false;

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(mDeviceExtensions.begin(), mDeviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    hasExtensionSupport = requiredExtensions.empty();

    // Result

    return hasQueueFamilySupport && hasSurfaceSupport && hasExtensionSupport;
}

const VkPhysicalDevice& PhysicalDeviceManager::getDevice() const {
	return mVkPhysicalDevice;
}