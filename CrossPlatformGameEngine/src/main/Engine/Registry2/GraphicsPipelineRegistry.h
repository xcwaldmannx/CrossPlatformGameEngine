#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../Pipeline/GraphicsPipeline/GraphicsPipeline.h"
#include "../Pipeline/GraphicsPipeline/GraphicsPipelineFactory.h"

#include <any>
#include <unordered_map>

namespace ascen
{

    class GraphicsPipelineRegistry : public Registry_I<registry::GraphicsPipelineEntry, GraphicsPipelinePtr>
    {
    public:
        GraphicsPipelineRegistry(
            const VkDevice device,
            const SwapchainPtr& swapchain,
            const GraphicsPipelineFactory& graphicsPipelineFactory,
            std::unordered_map<uint64_t, std::shared_ptr<Handle_I>>* idToResource) :
            mDevice(device),
            mSwapchain(swapchain),
            mGraphicsPipelineFactory(graphicsPipelineFactory),
            mIdToResource(idToResource) {}

        void reconstruct(const registry::GraphicsPipelineEntry& entry, GraphicsPipelinePtr& resource) override
        {
            if (resource) resource->destroy(mDevice);

            std::vector<DescriptorSetLayoutPtr> layouts;

            for (auto layoutId : entry.mDescriptorSetLayoutIds)
            {
                const auto& layout = std::dynamic_pointer_cast<DescriptorSetLayout>(mIdToResource->at(layoutId));
                layouts.push_back(layout);
            }

            const auto& vertex = std::dynamic_pointer_cast<Vertex>(mIdToResource->at(entry.mVertexId));
            const auto& renderPass = std::dynamic_pointer_cast<RenderPass>(mIdToResource->at(entry.mRenderPassId));

            resource = mGraphicsPipelineFactory.create(
                entry.mParams,
                entry.mVertexShaderPath,
                entry.mPixelShaderPath,
                vertex,
                layouts,
                mSwapchain,
                renderPass);
        }

    private:
        const VkDevice mDevice;
        const SwapchainPtr& mSwapchain;
        const GraphicsPipelineFactory& mGraphicsPipelineFactory;

        std::unordered_map<uint64_t, std::shared_ptr<Handle_I>>* mIdToResource = nullptr;
    };

}
