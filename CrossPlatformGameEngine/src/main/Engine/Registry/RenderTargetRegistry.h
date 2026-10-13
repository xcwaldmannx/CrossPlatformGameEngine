#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../RenderTarget/RenderTargetFactory.h"
#include "../RenderTarget/RenderTarget.h"

namespace ascen
{

    class RenderTargetRegistry : public Registry_I<registry::RenderTargetEntry, RenderTargetPtr>
    {
    public:
        RenderTargetRegistry(
        const VkDevice device,
        const RenderTargetFactory& renderTargetFactory,
        std::unordered_map<uint64_t, std::shared_ptr<Handle_I>>* idToResource) :
        mDevice(device),
        mRenderTargetFactory(renderTargetFactory),
        mIdToResource(idToResource) {}

        void reconstruct(const registry::RenderTargetEntry& entry, RenderTargetPtr& resource) override
        {
            if (resource) resource->destroy(mDevice);

            std::vector<TexturePtr> textures;
            if (!entry.mTextureIds.empty())
            {
                for (const uint64_t id : entry.mTextureIds)
                {
                    const TexturePtr& texture = std::dynamic_pointer_cast<Texture>(mIdToResource->at(id));
                    textures.push_back(texture);
                }
            }

            resource = mRenderTargetFactory.create(textures);
        }

    private:
        const VkDevice mDevice;
        const RenderTargetFactory& mRenderTargetFactory;

        std::unordered_map<uint64_t, std::shared_ptr<Handle_I>>* mIdToResource = nullptr;
    };

}
