#pragma once

#include "../Core/Types.h"
#include "../Handle/Handle.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    class Swapchain;

    class RenderTarget : public Handle<void*>
    {
    public:
        RenderTarget(
            const VkDevice device,
            const Format format,
            const std::vector<VkImage>& images);

        void destroy(const VkDevice device);

        void resize(const VkDevice device);

        std::vector<VkImageView>& getImageViews();

    private:
        void createImageViews(const VkDevice device);
        void destroyImageViews(const VkDevice device);

    private:
        const Format mFormat;
        std::vector<VkImage> mImages;
        std::vector<VkImageView> mImageViews;
    };

}