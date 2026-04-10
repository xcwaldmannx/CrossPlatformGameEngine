#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    class RenderTarget
    {
    public:
        RenderTarget(
            const VkDevice device,
            const VkFormat imageFormat,
            const std::vector<VkImage>& images);

        void destroy(const VkDevice device) const;

        void resize(const VkDevice device);

        std::vector<VkImageView>& getImageViews();

    private:
        void createImageViews(const VkDevice device);
        void destroyImageViews(const VkDevice device) const;

    private:
        const VkFormat mImageFormat;
        std::vector<VkImage> mImages;
        std::vector<VkImageView> mImageViews;
    };

}