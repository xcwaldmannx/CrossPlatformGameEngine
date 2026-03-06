#include "FrameGraph.h"

#include "../Registry/FramePass/FramePassRegistryBackend.h"

#include <algorithm>
#include <chrono>
#include <iostream>

using namespace ascen;

FrameGraph::FrameGraph(const FramePassRegistry& framePassRegistry) :
	mFramePassRegistry(framePassRegistry) {}

void FrameGraph::compile()
{
    mExecutions.clear();

    const auto& framePasses = FramePassRegistryBackend::getFramePasses(mFramePassRegistry);

    std::unordered_map<std::string, unsigned int> lastWriter;
    std::vector<FrameGraphNode> nodes;
    nodes.reserve(framePasses.size());

    unsigned int passIndex = 0;

    for (auto& [name, framePass] : framePasses)
    {
        nodes.push_back(FrameGraphNode{});
        FrameGraphNode& node = nodes.back();
        node.mFramePass = framePass;

        switch (framePass->mType)
        {
            case FramePassType::NONE:
                break;

            case FramePassType::CPU:
                break;

            case FramePassType::COMPUTE:
            case FramePassType::GRAPHICS:
            {
                auto gpuFramePass = std::static_pointer_cast<GpuFramePass>(framePass);

                const auto& resources = gpuFramePass->mResources;

                for (const auto& resource : resources)
                {
                    auto it = lastWriter.find(resource.mName);
                    if (it != lastWriter.end()) node.mDependencies.push_back(it->second);

                    if (resource.mAccess == ResourceAccess::WRITE)
                    {
                        lastWriter[resource.mName] = passIndex;
                    }
                }
            }
        }

        {
            auto& deps = node.mDependencies;
            std::sort(deps.begin(), deps.end());
            deps.erase(std::unique(deps.begin(), deps.end()), deps.end());
        }

        passIndex++;
    }

    const auto order = topoSort(nodes);

    // Example: store the ordered passes
    mExecutions.reserve(order.size());
    for (unsigned int idx : order)
    {
        mExecutions.push_back(nodes[idx].mFramePass);
    }

    // debug logging below
    /*

    passIndex = 0;

    for (const auto& exec : mExecutions)
    {
        const auto& pass = std::reinterpret_pointer_cast<GpuFramePass>(exec);

        std::cout << "pass: " << passIndex << std::endl << "resources:" << std::endl;

        for (const auto& resource : pass->mResources)
        {
            std::string access = (resource.mAccess == ResourceAccess::WRITE) ? "WRITE" : "READ";
            std::cout << "    " << resource.mName << ": " << access << std::endl;
        }

        std::cout << std::endl;


        passIndex++;

    }
    */

}

const std::vector<FramePassPtr>& FrameGraph::getExecutions() const
{
	return mExecutions;
}
