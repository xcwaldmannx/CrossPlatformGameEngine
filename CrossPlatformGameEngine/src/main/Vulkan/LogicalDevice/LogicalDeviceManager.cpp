#include "LogicalDeviceManager.h"

#include <stdexcept>

#include <vector>
#include <set>

using namespace vulkan;

const VkDevice& LogicalDeviceManager::getDevice() const {
	return mVkDevice;
}

VkResult LogicalDeviceManager::create(const VkPhysicalDevice& physicalDevice, const QueueFamilyManager& queueFamilyManager) {
    uint32_t graphicsFamilyIndex = queueFamilyManager.getGraphicsFamilyIndex();
    uint32_t presentFamilyIndex = queueFamilyManager.getPresentFamilyIndex();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { graphicsFamilyIndex, presentFamilyIndex };

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

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(mDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &mVkDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    return VK_SUCCESS;
}

void LogicalDeviceManager::destroy() {
	vkDestroyDevice(mVkDevice, nullptr);
}
