#include "PhysicalDevice.h"

#include <stdexcept>

#include <optional>
#include <set>
#include <string>

using namespace ascen;

VkPhysicalDevice PhysicalDevice::get(VkInstance instance, VkSurfaceKHR surface)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());



    for (const auto& device : devices) {
        if (checkDeviceSupport(device, surface)) {
            return device;
        }
    }

       throw std::runtime_error("failed to find a suitable GPU!");
}

bool PhysicalDevice::checkDeviceSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
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
    VkBool32 fillModeNonSolidSupport = supportedFeatures.fillModeNonSolid;

    // Result

    return hasQueueFamilySupport && hasSurfaceSupport && hasExtensionSupport && anisotropicSupport && fillModeNonSolidSupport;
}

VkFormat PhysicalDevice::findDepthFormat(VkPhysicalDevice physicalDevice)
{
    std::vector<VkFormat> candidates =
    {
        VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
    };

    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("Failed to find suitable depth format!");
}

const VkPhysicalDeviceLimits& PhysicalDevice::getLimits(VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceProperties properties;

    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    return properties.limits;


}
