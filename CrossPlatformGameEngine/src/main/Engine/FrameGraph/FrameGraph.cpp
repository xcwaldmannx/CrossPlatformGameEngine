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
            case FramePassType::TRANSFER:
            {
                auto gpuFramePass = std::static_pointer_cast<GpuFramePass>(framePass);
                const auto& readBuffers  = gpuFramePass->mReadBuffers;
                const auto& writeBuffers = gpuFramePass->mWriteBuffers;

                for (const auto& readBuffer : readBuffers)
                {
                    auto it = lastWriter.find(readBuffer);
                    if (it != lastWriter.end()) node.mDependencies.push_back(it->second);
                }

                for (const auto& writeBuffer : writeBuffers)
                {
                    auto it = lastWriter.find(writeBuffer);
                    if (it != lastWriter.end()) node.mDependencies.push_back(it->second);

                    lastWriter[writeBuffer] = passIndex;
                }

                break;
            }
        }

        {
            auto& deps = node.mDependencies;
            std::sort(deps.begin(), deps.end());
            deps.erase(std::unique(deps.begin(), deps.end()), deps.end());
        }

        passIndex++;
    }

    auto order = topoSort(nodes);

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

        std::cout << "pass: " << passIndex << std::endl << "inputs:" << std::endl;

        for (const auto& readBuffer : pass->mReadBuffers)
        {
            std::cout << readBuffer << ", " << std::endl;
        }

        std::cout << "outputs:" << std::endl;

        for (const auto& writeBuffer : pass->mWriteBuffers)
        {
            std::cout << writeBuffer << ", " << std::endl;
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
