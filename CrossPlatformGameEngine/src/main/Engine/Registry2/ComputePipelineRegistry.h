#pragma once

#include "Registry_I.h"
#include "../Core/Types.h"
#include "../Pipeline/ComputePipeline/ComputePipeline_I.h"
#include "../Pipeline/ComputePipeline/ComputePipelineFactory.h"

#include <any>
#include <unordered_map>

namespace ascen
{

    class ComputePipelineRegistry : public Registry_I<registry::ComputePipelineEntry, ComputePipelinePtr>
    {
    public:
        ComputePipelineRegistry(
            const VkDevice device,
            const ComputePipelineFactory& computePipelineFactory,
            std::unordered_map<uint64_t, registry::Resource>* idToResource) :
            mDevice(device),
            mComputePipelineFactory(computePipelineFactory),
            mIdToResource(idToResource) {}

        void reconstruct(const registry::ComputePipelineEntry& entry, ComputePipelinePtr& resource) override
        {
            if (resource) resource->destroy(mDevice);

            std::vector<DescriptorSetLayoutPtr> layouts;

            for (auto layoutId : entry.mDescriptorSetLayoutIds)
            {
                const auto& layout = std::any_cast<DescriptorSetLayoutPtr>(mIdToResource->at(layoutId));
                layouts.push_back(layout);
            }

            resource = mComputePipelineFactory.create(
                entry.mParams,
                entry.mComputeShaderPath,
                layouts);

            // implement push constants later
        }

    private:
        const VkDevice mDevice;
        const ComputePipelineFactory& mComputePipelineFactory;

        std::unordered_map<uint64_t, registry::Resource>* mIdToResource = nullptr;
    };

}
