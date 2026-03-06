#pragma once

#include "../Registry/Pipeline/PipelineRegistryBackend.h"
#include "../Registry/Descriptor/DescriptorRegistryBackend.h"
#include "../Registry/Resource/ResourceRegistryBackend.h"

namespace ascen
{

    struct RecorderContext
    {

    };

    class CommandRecorder_I
    {
    public:
        CommandRecorder_I(
            PipelineRegistry& pipelineRegistry,
            DescriptorRegistry& descriptorRegistry,
            ResourceRegistry& resourceRegistry) :
            mPipelineRegistry(pipelineRegistry),
            mDescriptorRegistry(descriptorRegistry),
            mResourceRegistry(resourceRegistry) {}

    protected:
        PipelineRegistry& mPipelineRegistry;
        DescriptorRegistry& mDescriptorRegistry;
        ResourceRegistry& mResourceRegistry;
    };

}
