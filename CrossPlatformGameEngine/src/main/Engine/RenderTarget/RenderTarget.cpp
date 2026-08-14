#include "RenderTarget.h"

#include <stdexcept>

#include "../Resource/Texture/Texture.h"

using namespace ascen;

RenderTarget::RenderTarget(
    const VkDevice device,
    const std::vector<TexturePtr>& textures) :
    mTextures(textures) {}

void RenderTarget::destroy(const VkDevice device) {}

std::vector<VkImageView> RenderTarget::getImageViews()
{
    std::vector<VkImageView> imageViews;

    for (const TexturePtr& texture : mTextures)
    {
        imageViews.push_back(texture->handle());
    }

    return imageViews;
}
