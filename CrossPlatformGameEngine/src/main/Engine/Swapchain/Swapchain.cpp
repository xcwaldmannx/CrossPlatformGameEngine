#include "Swapchain.h"

#include <algorithm>
#include <stdexcept>

#include <array>
#include <limits>

#include "../Resource/Texture/Texture.h"

using namespace ascen;

Swapchain::Swapchain(
    GLFWwindow* window,
    const VkPhysicalDevice physicalDevice,
    const VkDevice device,
    const VkSurfaceKHR surface,
    const uint32_t graphicsFamily,
    const uint32_t presentFamily) :
        mPhysicalDevice(physicalDevice),
        mDevice(device)
{
	SwapchainSupportDetails swapChainSupport = querySwapchainSupport(physicalDevice, surface);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.mFormats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.mPresentModes);

	mExtent = chooseSwapExtent(window, swapChainSupport.mCapabilities);

    mImageFormat = surfaceFormat.format;

    mImageCount = swapChainSupport.mCapabilities.minImageCount;

    if (swapChainSupport.mCapabilities.maxImageCount > 0 &&
        mImageCount > swapChainSupport.mCapabilities.maxImageCount)
    {
        mImageCount = swapChainSupport.mCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = mImageCount;
    createInfo.imageFormat = mImageFormat;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = mExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = swapChainSupport.mCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = nullptr;

    uint32_t queueFamilyIndices[] = { graphicsFamily, presentFamily };

    if (graphicsFamily != presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &mHandle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device, mHandle, &mImageCount, nullptr);
    mImages.resize(mImageCount);
    vkGetSwapchainImagesKHR(device, mHandle, &mImageCount, mImages.data());
}

void Swapchain::destroy(VkDevice device)
{
    destroyFrameBuffers(device);
    destroyImageViews(device);
    vkDestroySwapchainKHR(device, mHandle, nullptr);
}

const VkExtent2D& Swapchain::getExtent() const
{
    return mExtent;
}

uint32_t Swapchain::getImageCount() const
{
    return mImageCount;
}

const std::vector<VkImage>& Swapchain::getImages() const
{
    return mImages;
}

VkFormat Swapchain::getImageFormat() const
{
    return mImageFormat;
}

VkFramebuffer_T* Swapchain::getFrameBuffer(uint32_t imageIndex) const
{
    const auto& frameBuffer = mFrameBuffers.at(imageIndex);
    //mCurrentFrameIndex = (mCurrentFrameIndex + 1) % mFrameBuffers.size();
    return frameBuffer;
}

SwapchainSupportDetails Swapchain::querySwapchainSupport(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface)
{
    SwapchainSupportDetails details{};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.mCapabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.mFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.mFormats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.mPresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice, surface, &presentModeCount, details.mPresentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseSwapExtent(
    GLFWwindow* window,
    const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
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

void Swapchain::setDepthTexture(const TexturePtr& texture)
{
    mDepthTexture = texture;
}

void Swapchain::createFrameBuffers(
    const VkDevice device,
    const VkRenderPass renderPass)
{
    destroyImageViews(device);
    createImageViews(device);

    destroyFrameBuffers(mDevice);

    mFrameBuffers.resize(mImageViews.size());

    for (size_t i = 0; i < mImageViews.size(); i++)
    {
        std::array<VkImageView, 2> attachments =
        {
            mImageViews[i],
            mDepthTexture->handle(),
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = mExtent.width;
        framebufferInfo.height = mExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &mFrameBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void Swapchain::destroyFrameBuffers(const VkDevice device) const
{
    for (const auto& framebuffer : mFrameBuffers)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
}

void Swapchain::createImageViews(const VkDevice device)
{
    mImageViews.resize(mImages.size());

    for (size_t i = 0; i < mImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = mImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = mImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &createInfo, nullptr, &mImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void Swapchain::destroyImageViews(const VkDevice device) const
{
    for (auto imageView : mImageViews)
    {
        vkDestroyImageView(device, imageView, nullptr);
    }
}
