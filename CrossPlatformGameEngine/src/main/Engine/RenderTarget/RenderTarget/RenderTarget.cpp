#include "RenderTarget.h"

#include "../../Swapchain/Swapchain.h"

#include <stdexcept>

using namespace ascen;

RenderTarget::RenderTarget(
    const VkDevice device,
    const Format format,
    const std::vector<VkImage>& images) :
    mFormat(format),
    mImages(images)
{
    resize(device);
}

void RenderTarget::destroy(const VkDevice device)
{
    if (!mImageViews.empty()) destroyImageViews(device);
}

void RenderTarget::resize(const VkDevice device)
{
    if (!mImageViews.empty()) destroyImageViews(device);

    createImageViews(device);
}

std::vector<VkImageView>& RenderTarget::getImageViews()
{
    return mImageViews;
}

void RenderTarget::createImageViews(const VkDevice device)
{
    mImageViews.resize(mImages.size());

    for (size_t i = 0; i < mImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = mImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = static_cast<VkFormat>(mFormat);
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
            throw std::runtime_error("Failed to create image views!");
        }
    }
}

void RenderTarget::destroyImageViews(const VkDevice device)
{
    for (const auto imageView : mImageViews)
    {
        vkDestroyImageView(device, imageView, nullptr);
    }
    mImageViews.clear();
}
