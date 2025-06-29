#include "QueueFamilyManager.h"

#include <cassert>

#include <vector>

using namespace vulkan;

void QueueFamilyManager::init(
    const VkPhysicalDevice& physicalDevice,
    const VkSurfaceKHR& surface
) {
    mPhysicalDevice = physicalDevice;
    mSurface = surface;
    mIsInitialized = true;
}

void QueueFamilyManager::updateQueueFamilies() {
    assert(mIsInitialized && "QueueFamilyManager not initialized.");

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            mGraphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, mSurface, &presentSupport);

        if (presentSupport) {
            mPresentFamily = i;
        }

        if (mGraphicsFamily.has_value() && mPresentFamily.has_value()) {
            break;
        }

        i++;
    }
}

uint32_t QueueFamilyManager::getGraphicsFamilyIndex() const {
    return mGraphicsFamily.value();
}

uint32_t QueueFamilyManager::getPresentFamilyIndex() const {
    return mPresentFamily.value();
}

VkQueue QueueFamilyManager::getDeviceQueue(const VkDevice& logicalDevice, uint32_t index) {
    assert(mIsInitialized && "QueueFamilyManager not initialized.");

    VkQueue queue;
    vkGetDeviceQueue(logicalDevice, index, 0, &queue);

    return queue;
}
