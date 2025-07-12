#include "PhysicalDeviceHandler.h"

#include <stdexcept>

#include <optional>
#include <set>
#include <string>

/*
Begin PhysicalDeviceHandler Implementation
*/

namespace ascen {

    static const std::vector<const char*> mDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    static bool checkDeviceSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
        // QueueFamily Support
        bool hasQueueFamilySupport = false;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        std::optional<uint32_t> graphicsFamilyIndex;
        std::optional<uint32_t> presentFamilyIndex;

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsFamilyIndex = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

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
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        hasSurfaceSupport = (formatCount != 0 && presentModeCount != 0) ? true : false;

        // Extension Support
        bool hasExtensionSupport = false;

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(mDeviceExtensions.begin(), mDeviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        hasExtensionSupport = requiredExtensions.empty();

        // other support
        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        VkBool32 anisotropicSupport = supportedFeatures.samplerAnisotropy;

        // Result

        return hasQueueFamilySupport && hasSurfaceSupport && hasExtensionSupport && anisotropicSupport;
    }

    void pickPhysicalDevice(VulkanInstance& instance, Surface& surface, PhysicalDevice* physicalDevice) {

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance.mInstance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance.mInstance, &deviceCount, devices.data());



        for (const auto& device : devices) {
            if (checkDeviceSupport(device, surface.mSurface)) {
                physicalDevice->mDevice = device;
                break;
            }
        }

        if (physicalDevice == nullptr) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

}

/*
End PhysicalDeviceHandler Implementation
*/
