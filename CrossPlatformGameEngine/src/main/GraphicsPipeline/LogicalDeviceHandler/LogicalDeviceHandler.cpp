#include "LogicalDeviceHandler.h"

#include <stdexcept>

#include <set>

/*
Begin LogicalDeviceHandler Implementation
*/

namespace ascen {

    static const std::vector<const char*> mDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    void createLogicalDevice(
        PhysicalDevice& physicalDevice,
        uint32_t graphicsFamily,
        uint32_t presentFamily,
        LogicalDevice* logicalDevice) {

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

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(mDeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

        if (vkCreateDevice(
            physicalDevice.mDevice, &createInfo, nullptr, &logicalDevice->mDevice) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }
    }

    void destroyLogicalDevice(LogicalDevice& logicalDevice) {
        vkDestroyDevice(logicalDevice.mDevice, nullptr);
    }

}

/*
End LogicalDeviceHandler Implementation
*/
