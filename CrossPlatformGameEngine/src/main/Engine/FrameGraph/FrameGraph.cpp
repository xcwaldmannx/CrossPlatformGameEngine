#include "FrameGraph.h"

#include "../Registry/FramePass/FramePassRegistryBackend.h"

#include <algorithm>
#include <chrono>
#include <iostream>

#include "../Resource/Barrier/Barrier.h"

using namespace ascen;

FrameGraph::FrameGraph(
    const FramePassRegistry& framePassRegistry,
    const ResourceRegistry& resourceRegistry) :
	mFramePassRegistry(framePassRegistry),
    mResourceRegistry(resourceRegistry) {}

void FrameGraph::compile()
{
    mExecutions.clear();

    const auto& framePasses = FramePassRegistryBackend::getFramePasses(mFramePassRegistry);

    std::vector<FrameGraphNode> nodes;
    nodes.reserve(framePasses.size());

    std::unordered_map<std::string, ResourceLastUsage> resourceLastUsage;

    unsigned int passIndex = 0;

    for (auto& [name, framePass] : framePasses)
    {
        nodes.push_back(FrameGraphNode{});
        FrameGraphNode& node = nodes.back();
        node.mName = name;
        node.mFramePass = framePass;

        switch (framePass->mType)
        {
            case FramePassType::NONE:
            case FramePassType::CPU:
                break;

            case FramePassType::COMPUTE:
            case FramePassType::GRAPHICS:
            {
                auto gpuFramePass = std::static_pointer_cast<GpuFramePass>(framePass);
                const auto& resources = gpuFramePass->mResources;

                node.mResources = resources;

                for (const auto& resource : resources)
                {
                    auto it = resourceLastUsage.find(resource.mName);

                    if (it != resourceLastUsage.end())
                    {
                        ResourceLastUsage& lastUse = it->second;

                        node.mDependencies.push_back(lastUse.mLastUsageNode);

                        if (needsBarrier(lastUse.mResource, resource))
                        {
                            switch (resource.mUsage)
                            {
                                case ResourceUsage::BUFFER_VERTEX:
                                case ResourceUsage::BUFFER_INDEX:
                                case ResourceUsage::BUFFER_UNIFORM:
                                case ResourceUsage::BUFFER_STORAGE:
                                case ResourceUsage::BUFFER_INDIRECT:
                                case ResourceUsage::BUFFER_TRANSFER_SRC:
                                case ResourceUsage::BUFFER_TRANSFER_DST:

                                    break;

                                case ResourceUsage::IMAGE_SAMPLED:
                                case ResourceUsage::IMAGE_STORAGE:
                                case ResourceUsage::IMAGE_COLOR_ATTACH:
                                case ResourceUsage::IMAGE_DEPTH_ATTACH:
                                case ResourceUsage::IMAGE_TRANSFER_SRC:
                                case ResourceUsage::IMAGE_TRANSFER_DST:
                                case ResourceUsage::IMAGE_PRESENT:
                                    break;
                            }
                        }

                        lastUse.mResource = resource;
                        lastUse.mLastUsageNode = passIndex;
                    }
                    else
                    {
                        resourceLastUsage[resource.mName] = { resource, passIndex };
                    }
                }

                break;
            }

            default:
                break;
        }

        auto& deps = node.mDependencies;
        std::sort(deps.begin(), deps.end());
        deps.erase(std::unique(deps.begin(), deps.end()), deps.end());

        passIndex++;
    }

    const auto order = topoSort(nodes);

    mExecutions.reserve(order.size());

    for (unsigned int idx : order)
    {
        mExecutions.push_back(nodes[idx].mFramePass);
    }

    // debug logging below

    passIndex = 0;

    std::cout << "BEGIN" << std::endl;

    for (const auto& exec : mExecutions)
    {
        const auto& pass = std::reinterpret_pointer_cast<GpuFramePass>(exec);

        std::cout << "\t" << pass->getStringType() << " PASS: " << passIndex << std::endl << "\tresources:" << std::endl;

        for (const auto& resource : pass->mResources)
        {
            std::string access = (resource.mAccess == ResourceAccess::WRITE) ? "WRITE" : "READ";
            std::cout << "\t\t" << resource.mName << ": " << access << std::endl;
        }

        std::cout << std::endl;


        passIndex++;

    }

    std::cout << "END" << std::endl;

}

const std::vector<FramePassPtr>& FrameGraph::getExecutions() const
{
	return mExecutions;
}

bool FrameGraph::needsBarrier(const GpuResource& previous, const GpuResource& current)
{
    if (static_cast<uint32_t>(previous.mAccess) & (static_cast<uint32_t>(ResourceAccess::WRITE) | static_cast<uint32_t>(ResourceAccess::READ_WRITE)) ||
        static_cast<uint32_t>(current.mAccess)  & (static_cast<uint32_t>(ResourceAccess::WRITE) | static_cast<uint32_t>(ResourceAccess::READ_WRITE)))
        return true;

    if (previous.mUsage != current.mUsage)
        return true;

    return false;
}
