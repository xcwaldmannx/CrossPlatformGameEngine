#include "RenderTargetRegistry.h"

#include "../../Core/VulkanContext.h"

using namespace ascen;

RenderTargetRegistry::RenderTargetRegistry(
    const VulkanContext& vulkanContext,
    const RenderContext& renderContext) :
    mDevice(vulkanContext.getDevice()),
    mRenderPassFactory(vulkanContext.getRenderPassFactory()),
    mRenderTargetFactory(vulkanContext.getRenderTargetFactory()) {}

void RenderTargetRegistry::registerRenderPass(const RenderPassEntry& entry)
{
    if (isRegistered(entry.mName))
    {
        throw std::runtime_error("A render resource with that name already exists!");
    }

    mRegisteredNames.push_back(entry.mName);
    mRenderPassEntries.emplace_back(std::move(entry));
}

void RenderTargetRegistry::registerRenderTarget(const RenderTargetEntry& entry)
{
    if (isRegistered(entry.mName))
    {
        throw std::runtime_error("A render resource with that name already exists!");
    }

    mRegisteredNames.push_back(entry.mName);
    mRenderTargetEntries.emplace_back(std::move(entry));
}

void RenderTargetRegistry::reconstruct()
{
    cleanup();

    for (const auto& entry : mRenderPassEntries)
    {
        mRenderPasses[entry.mName] = mRenderPassFactory.create(entry.mAttachments, entry.mSubPasses, entry.mSubPassDependencies);
    }

    for (const auto& entry : mRenderTargetEntries)
    {
        mRenderTargets[entry.mName] = mRenderTargetFactory.create(entry.mFormat, entry.mImages);
    }
}

void RenderTargetRegistry::cleanup()
{
    for (auto& [name, pass] : mRenderPasses)
    {
        if (pass) mRenderPasses.at(name)->destroy(mDevice);
    }
    mRenderPasses.clear();

    for (auto& [name, target] : mRenderTargets)
    {
        if (target) mRenderTargets.at(name)->destroy(mDevice);
    }
    mRenderTargets.clear();
}

bool RenderTargetRegistry::passExists(const std::string& name) const
{
    return (mRenderPasses.contains(name));
}

bool RenderTargetRegistry::targetExists(const std::string& name) const
{
    return (mRenderTargets.contains(name));
}

bool RenderTargetRegistry::isRegistered(const std::string& name) const
{
    for (const auto& registeredName : mRegisteredNames)
    {
        if (name == registeredName)
        {
            return true;
        }
    }

    return false;
}
