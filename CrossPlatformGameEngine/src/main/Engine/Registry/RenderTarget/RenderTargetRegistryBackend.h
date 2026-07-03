#pragma once

#include "RenderTargetRegistry.h"

namespace ascen
{

    class RenderTargetRegistryBackend
    {
    public:
        static const RenderPassPtr& getRenderPass(
            const RenderTargetRegistry& registry, const std::string& name);

        static const RenderTargetPtr& getRenderTarget(
            const RenderTargetRegistry& registry, const std::string& name);
    };

}
