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
            const std::vector<TexturePtr>& textures);

        void destroy(const VkDevice device);

        std::vector<VkImageView> getImageViews();

    private:
        std::vector<TexturePtr> mTextures;
    };

}