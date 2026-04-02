#include "FrameGraph.h"

#include "../Registry/FramePass/FramePassRegistryBackend.h"

#include <algorithm>
#include <chrono>
#include <iostream>

#include "../Registry/Resource/ResourceRegistryBackend.h"

using namespace ascen;

std::unordered_map<ResourceAccess, VkAccessFlags2> FrameGraph::sResourceAccessMap =
{
    { ResourceAccess::READ,  VK_ACCESS_2_SHADER_READ_BIT  },
    { ResourceAccess::WRITE, VK_ACCESS_2_SHADER_WRITE_BIT }
};

std::unordered_map<ResourceStage, VkPipelineStageFlags2> FrameGraph::sResourceStageMap =
{
    { ResourceStage::VERTEX,   VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT   },
    { ResourceStage::FRAGMENT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT },
    { ResourceStage::COMPUTE,  VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT  },
};

FrameGraph::FrameGraph(
    FramePassRegistry& framePassRegistry,
    const ResourceRegistry& resourceRegistry) :
	mFramePassRegistry(framePassRegistry),
    mResourceRegistry(resourceRegistry) {}

void FrameGraph::compile()
{
    mExecutions.clear();

    const auto framePasses = FramePassRegistryBackend::getFramePasses(mFramePassRegistry);

    std::unordered_map<std::string, std::string> resourcePrevFramePass;
    std::unordered_map<std::string, ResourceAccess> resourcePrevAccess;
    std::unordered_map<std::string, ResourceStage> resourcePrevStage;

    for (auto& [name, framePass] : framePasses)
    {
        // insert a SyncFramePass for GPU frame passes
        if (framePass->mType == FramePassType::GRAPHICS || framePass->mType == FramePassType::COMPUTE)
        {
            const auto& gpuFramePass = std::static_pointer_cast<GpuFramePass>(framePass);
            const auto& resources = gpuFramePass->mResources;

            for (const auto& resource : resources)
            {
                if (resourcePrevAccess.contains(resource.mName))
                {
                    const auto prevAccess = resourcePrevAccess[resource.mName];
                    const auto currAccess = resource.mAccess;

                    if (prevAccess == ResourceAccess::READ  && currAccess == ResourceAccess::WRITE ||
                        prevAccess == ResourceAccess::WRITE && currAccess == ResourceAccess::READ  ||
                        prevAccess == ResourceAccess::WRITE && currAccess == ResourceAccess::WRITE)
                    {
                        const auto prevStage = resourcePrevStage.at(resource.mName);
                        const auto currStage = resource.mStage;

                        const auto srcAccess = sResourceAccessMap.at(prevAccess);
                        const auto srcStage = sResourceStageMap.at(prevStage);

                        const auto dstAccess = sResourceAccessMap.at(currAccess);
                        const auto dstStage = sResourceStageMap.at(currStage);

                        // insert SyncFramePass that will generate a barrier
                        std::string syncName = resourcePrevFramePass[resource.mName] + "__TO__" + name;

                        const auto& buffer = ResourceRegistryBackend::getBuffer(mResourceRegistry, resource.mName);
                        mFramePassRegistry.registerSync({ syncName, buffer->handle(), srcAccess, srcStage, dstAccess, dstStage });

                        mFramePassRegistry.reconstruct();

                        mExecutions.push_back(FramePassRegistryBackend::getFramePass(mFramePassRegistry, syncName));
                    }
                }

                resourcePrevAccess[resource.mName] = resource.mAccess;
                resourcePrevStage[resource.mName] = resource.mStage;
                resourcePrevFramePass[resource.mName] = name;
            }
        }

        mExecutions.push_back(framePass);
    }

    // debug logging below

#ifdef DEBUG
    unsigned int passIndex = 0;

    std::cout << "BEGIN" << std::endl;

    for (const auto& exec : mExecutions)
    {
        switch (exec->mType)
        {
            case FramePassType::GRAPHICS:
            case FramePassType::COMPUTE:
            {
                const auto& pass = std::reinterpret_pointer_cast<GpuFramePass>(exec);

                std::cout << "\t" << pass->getStringType() << " PASS: " << passIndex << std::endl << "\tresources:" << std::endl;

                for (const auto& resource : pass->mResources)
                {
                    std::string access = (resource.mAccess == ResourceAccess::WRITE) ? "WRITE" : "READ";
                    std::cout << "\t\t" << resource.mName << ": " << access << std::endl;
                }
                break;
            }
            case FramePassType::SYNC:
            {
                const auto& pass = std::reinterpret_pointer_cast<SyncFramePass>(exec);

                std::cout << "\t" << pass->getStringType() << " PASS: " << passIndex << std::endl;
            }
            default:
                break;
        }

        std::cout << std::endl;


        passIndex++;

    }

    std::cout << "END" << std::endl;
#endif
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
