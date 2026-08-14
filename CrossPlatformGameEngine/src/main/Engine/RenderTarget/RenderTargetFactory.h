#pragma once

#include "../Core/Values.h"
#include "../Core/Types.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

    class RenderTargetFactory
    {
    public:
        RenderTargetFactory(VkDevice device);

        RenderTargetPtr create(const std::vector<TexturePtr>& textures) const;

    private:
        VkDevice mDevice;
    };

}
