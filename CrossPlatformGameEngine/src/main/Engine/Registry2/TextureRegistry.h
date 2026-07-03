#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../Resource/Texture/TextureFactory.h"
#include "../Resource/Texture/Texture.h"

namespace ascen
{

    class TextureRegistry : public Registry_I<registry::TextureEntry, TexturePtr>
    {
    public:
        TextureRegistry(
            const VkDevice device,
            const CommandPoolPtr& commandPool,
            const TextureFactory& textureFactory) :
            mDevice(device),
            mCommandPool(commandPool),
            mTextureFactory(textureFactory) {}

        void reconstruct(const registry::TextureEntry& entry, TexturePtr& resource) override
        {
            if (resource) resource->destroy(mDevice);

            switch (entry.mType)
            {
                case TextureType::IMAGE:
                    resource = mTextureFactory.createImage(
                        mCommandPool,
                        entry.mWidth,
                        entry.mHeight,
                        entry.mLayers);
                    break;
                case TextureType::DEPTH:
                    resource = mTextureFactory.createDepth(
                        mCommandPool,
                        entry.mWidth,
                        entry.mHeight);
                    break;
                case TextureType::NONE:
                default:
                    break;
            }
        }

    private:
        const VkDevice mDevice;
        const CommandPoolPtr& mCommandPool;
        const TextureFactory& mTextureFactory;
    };

}
