#include "SwapchainHandler.h"

#include <algorithm>
#include <stdexcept>

/*
Begin SwapchainHandler Implementation
*/

namespace ascen {

    static void createImageViews(LogicalDevice& logicalDevice, Swapchain& swapchain) {
        swapchain.mImageViews.resize(swapchain.mImages.size());

        for (size_t i = 0; i < swapchain.mImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapchain.mImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapchain.mImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(
                logicalDevice.mDevice, &createInfo, nullptr, &swapchain.mImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    static SwapchainSupportDetails querySwapchainSupport(
        VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {

        SwapchainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.mCapabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.mFormats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.mFormats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.mPresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                physicalDevice, surface, &presentModeCount, details.mPresentModes.data());
        }

        return details;
    }

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    static VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
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

            actualExtent.width = 
                std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = 
                std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void createSwapchain(
        GLFWwindow* window,
        PhysicalDevice& physicalDevice,
        LogicalDevice& logicalDevice,
        Surface& surface,
        uint32_t graphicsFamily,
        uint32_t presentFamily,
        Swapchain& swapchain) {

        SwapchainSupportDetails swapChainSupport = querySwapchainSupport(physicalDevice.mDevice, surface.mSurface);
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.mFormats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.mPresentModes);
        VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.mCapabilities);

        uint32_t imageCount = swapChainSupport.mCapabilities.minImageCount + 1;

        if (swapChainSupport.mCapabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.mCapabilities.maxImageCount) {
            imageCount = swapChainSupport.mCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface.mSurface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.preTransform = swapChainSupport.mCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = nullptr;

        uint32_t queueFamilyIndices[] = { graphicsFamily, presentFamily };

        if (graphicsFamily != presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        if (vkCreateSwapchainKHR(logicalDevice.mDevice, &createInfo, nullptr, &swapchain.mSwapchain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(logicalDevice.mDevice, swapchain.mSwapchain, &imageCount, nullptr);
        swapchain.mImages.resize(imageCount);
        vkGetSwapchainImagesKHR(logicalDevice.mDevice, swapchain.mSwapchain, &imageCount, swapchain.mImages.data());

        swapchain.mImageFormat = surfaceFormat.format;
        swapchain.mExtent = extent;

        createImageViews(logicalDevice, swapchain);
    }

    void recreateSwapchain(
        GLFWwindow* window,
        PhysicalDevice& physicalDevice,
        LogicalDevice& logicalDevice,
        Surface& surface,
        uint32_t graphicsFamily,
        uint32_t presentFamily,
        Swapchain& swapchain) {

        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(logicalDevice.mDevice);

        destroySwapchain(logicalDevice, swapchain);

        createSwapchain(
            window,
            physicalDevice,
            logicalDevice,
            surface,
            graphicsFamily,
            presentFamily,
            swapchain);
    }

    void destroySwapchain(LogicalDevice& logicalDevice, Swapchain& swapchain) {
        for (auto framebuffer : swapchain.mFrameBuffers) {
            vkDestroyFramebuffer(logicalDevice.mDevice, framebuffer, nullptr);
        }

        for (auto imageView : swapchain.mImageViews) {
            vkDestroyImageView(logicalDevice.mDevice, imageView, nullptr);
        }

        vkDestroySwapchainKHR(logicalDevice.mDevice, swapchain.mSwapchain, nullptr);
    }

    void createFrameBuffers(
        LogicalDevice& logicalDevice,
        Swapchain& swapchain,
        RenderPass& renderpass,
        Texture& depthTexture) {
       swapchain.mFrameBuffers.resize(swapchain.mImageViews.size());

        for (size_t i = 0; i < swapchain.mImageViews.size(); i++) {
            std::array<VkImageView, 2> attachments = {
                swapchain.mImageViews[i],
                depthTexture.mView
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderpass.mRenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapchain.mExtent.width;
            framebufferInfo.height = swapchain.mExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(
                logicalDevice.mDevice, &framebufferInfo, nullptr, &swapchain.mFrameBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void destroyFrameBuffers(LogicalDevice& logicalDevice, Swapchain& swapchain) {
        for (auto framebuffer : swapchain.mFrameBuffers) {
            vkDestroyFramebuffer(logicalDevice.mDevice, framebuffer, nullptr);
        }
    }

}

/*
End SwapchainHandler Implementation
*/
