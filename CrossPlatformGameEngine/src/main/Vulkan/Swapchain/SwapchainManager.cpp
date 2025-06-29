#include "SwapchainManager.h"

#include "../QueueFamily/QueueFamilyManager.h"

#include <algorithm>
#include <stdexcept>

using namespace vulkan;

VkResult SwapchainManager::create(GLFWwindow* window,
    const QueueFamilyManager& queueFamilyManager,
    const VkPhysicalDevice& physicalDevice,
    const VkDevice& logicalDevice,
    const VkSurfaceKHR& surface) {
    SwapchainSupportDetails swapChainSupport = querySwapchainSupport(physicalDevice, surface);
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = nullptr;

    uint32_t graphicsFamilyIndex = queueFamilyManager.getGraphicsFamilyIndex();
    uint32_t presentFamilyIndex = queueFamilyManager.getPresentFamilyIndex();

    uint32_t queueFamilyIndices[] = { graphicsFamilyIndex, presentFamilyIndex };

    if (graphicsFamilyIndex != presentFamilyIndex) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &mVkSwapchain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(logicalDevice, mVkSwapchain, &imageCount, nullptr);
    mSwapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, mVkSwapchain, &imageCount, mSwapchainImages.data());

    mVkSwapchainImageFormat = surfaceFormat.format;
    mVkSwapchainExtent = extent;

    createImageViews(logicalDevice);

    return VK_SUCCESS;
}

VkResult SwapchainManager::recreate(GLFWwindow* window,
    const QueueFamilyManager& queueFamilyManager,
    const VkPhysicalDevice& physicalDevice,
    const VkDevice& logicalDevice,
    const VkSurfaceKHR& surface,
    const RenderPassManager& renderpass) {

    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(logicalDevice);

    destroy(logicalDevice);

    VkResult result = create(window, queueFamilyManager, physicalDevice, logicalDevice, surface);

    if (result == VK_SUCCESS) {
        result = createFrameBuffers(logicalDevice, renderpass);
    }
    else {
        throw std::runtime_error("Couldn't recreate swapchain.");
    }

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Couldn't recreate frame buffers.");
    }

    return VK_SUCCESS;
}

void SwapchainManager::destroy(const VkDevice& logicalDevice) {
    for (auto framebuffer : mSwapchainFrameBuffers) {
        vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
    }

    for (auto imageView : mSwapchainImageViews) {
        vkDestroyImageView(logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(logicalDevice, mVkSwapchain, nullptr);
}

const VkSwapchainKHR& SwapchainManager::getSwapchain() const {
    return mVkSwapchain;
}

const VkImage& SwapchainManager::getImage(size_t index) const {
    return mSwapchainImages[index];
}

const std::vector<VkImage>& SwapchainManager::getImages() const {
    return mSwapchainImages;
}

const VkFormat& SwapchainManager::getFormat() const {
    return mVkSwapchainImageFormat;
}

const VkExtent2D& SwapchainManager::getExtent() const {
    return mVkSwapchainExtent;
}

const VkImageView& SwapchainManager::getImageView(size_t index) const {
    return mSwapchainImageViews[index];
}

const std::vector<VkImageView>& SwapchainManager::getImageViews() const {
    return mSwapchainImageViews;
}

const VkFramebuffer& SwapchainManager::getFrameBuffer(size_t index) const {
    return mSwapchainFrameBuffers[index];
}

const std::vector<VkFramebuffer>& SwapchainManager::getFrameBuffers() const {
    return mSwapchainFrameBuffers;
}

SwapchainSupportDetails SwapchainManager::querySwapchainSupport(const VkPhysicalDevice& physicalDevice,
    const VkSurfaceKHR& surface) const {
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR SwapchainManager::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR SwapchainManager::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapchainManager::chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

VkResult SwapchainManager::createImageViews(const VkDevice& logicalDevice) {
    mSwapchainImageViews.resize(mSwapchainImages.size());

    for (size_t i = 0; i < mSwapchainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = mSwapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = mVkSwapchainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &mSwapchainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

VkResult SwapchainManager::createFrameBuffers(const VkDevice& logicalDevice, const RenderPassManager& renderpass) {
    mSwapchainFrameBuffers.resize(mSwapchainImageViews.size());

    for (size_t i = 0; i < mSwapchainImageViews.size(); i++) {
        VkImageView attachments[] = {
            mSwapchainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderpass.mVkRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = mVkSwapchainExtent.width;
        framebufferInfo.height = mVkSwapchainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &mSwapchainFrameBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    return VK_SUCCESS;
}

void SwapchainManager::destroyFrameBuffers(const VkDevice& logicalDevice) {
    for (auto framebuffer : mSwapchainFrameBuffers) {
        vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
    }
}
