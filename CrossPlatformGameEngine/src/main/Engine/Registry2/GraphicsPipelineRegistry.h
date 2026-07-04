#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../Pipeline/GraphicsPipeline/GraphicsPipeline_I.h"
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
            const RenderPassPtr& renderPass,
            const GraphicsPipelineFactory& graphicsPipelineFactory,
            std::unordered_map<uint64_t, registry::Resource>* idToResource) :
            mDevice(device),
            mSwapchain(swapchain),
            mRenderPass(renderPass),
            mGraphicsPipelineFactory(graphicsPipelineFactory),
            mIdToResource(idToResource) {}

        void reconstruct(const registry::GraphicsPipelineEntry& entry, GraphicsPipelinePtr& resource) override
        {
            if (resource) resource->destroy(mDevice);

            std::vector<DescriptorSetLayoutPtr> layouts;

            for (auto layoutId : entry.mDescriptorSetLayoutIds)
            {
                const auto& layout = std::any_cast<DescriptorSetLayoutPtr>(mIdToResource->at(layoutId));
                layouts.push_back(layout);
            }

            const auto& vertex = std::any_cast<VertexPtr>(mIdToResource->at(entry.mVertexId));

            resource = mGraphicsPipelineFactory.create(
                entry.mParams,
                entry.mVertexShaderPath,
                entry.mPixelShaderPath,
                vertex,
                layouts,
                mSwapchain,
                mRenderPass);
        }

    private:
        const VkDevice mDevice;
        const SwapchainPtr& mSwapchain;
        const RenderPassPtr& mRenderPass;
        const GraphicsPipelineFactory& mGraphicsPipelineFactory;

        std::unordered_map<uint64_t, registry::Resource>* mIdToResource = nullptr;
    };

}
