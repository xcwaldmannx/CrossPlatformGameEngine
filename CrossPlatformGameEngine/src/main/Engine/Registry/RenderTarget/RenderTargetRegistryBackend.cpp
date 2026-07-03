#include "RenderTargetRegistryBackend.h"

using namespace ascen;

const RenderPassPtr& RenderTargetRegistryBackend::getRenderPass(
    const RenderTargetRegistry& registry, const std::string& name)
{
    if (registry.passExists(name))
    {
        return registry.mRenderPasses.at(name);
    }

    return nullptr;
}

const RenderTargetPtr& RenderTargetRegistryBackend::getRenderTarget(
    const RenderTargetRegistry& registry, const std::string& name)
{
    if (registry.targetExists(name))
    {
        return registry.mRenderTargets.at(name);
    }

    return nullptr;
}
