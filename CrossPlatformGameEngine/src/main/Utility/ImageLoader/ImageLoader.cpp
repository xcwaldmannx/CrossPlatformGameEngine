#include "ImageLoader.h"

#include <stdexcept>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

void ImageLoader::loadImage(const char* filepath, RawImage* image) {
    // Set filepath (no need to cast away const)
    image->mFilepath = const_cast<char*>(filepath);

    int width, height, channels;
    stbi_uc* pixels = stbi_load(filepath, &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels) {
        throw std::runtime_error(std::string("failed to load texture image: ") + filepath);
    }

    image->mWidth = width;
    image->mHeight = height;
    image->mChannels = 4;
    size_t size = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
    image->mPixels.insert(image->mPixels.end(), pixels, pixels + size);

    stbi_image_free(pixels);
}
