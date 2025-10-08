#include "Device.h"

#include <stdexcept>

#include <set>
#include <vector>

using namespace ascen;

VkDevice Device::create(
    VkPhysicalDevice physicalDevice,
    uint32_t graphicsFamily,
    uint32_t presentFamily)
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { graphicsFamily, presentFamily };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;

    // this section allows access to all features, not just core 1.0 features.
    // This is not currently necessary but is here for later use.
    // -------------------------------------------------------------------------------------
    //VkPhysicalDeviceShaderDrawParametersFeatures ext_feature = {};
    //ext_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;

    //VkPhysicalDeviceFeatures2 physical_features2 = {};
    //physical_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    //physical_features2.pNext = &ext_feature;

    //vkGetPhysicalDeviceFeatures2(physicalDevice.mDevice, &physical_features2);

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    // createInfo.pNext = &physical_features2;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(sDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = sDeviceExtensions.data();

    VkDevice device;

    if (vkCreateDevice(
        physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    return device;
}

void Device::destroy(VkDevice device)
{
    vkDestroyDevice(device, nullptr);
}
