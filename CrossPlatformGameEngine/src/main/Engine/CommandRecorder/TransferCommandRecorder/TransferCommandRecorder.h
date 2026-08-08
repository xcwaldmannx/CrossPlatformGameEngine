#pragma once

#include "../CommandRecorder_I.h"
#include "../../Registry2/RegistryManager.h"
#include "../../Core/Types.h"

namespace ascen
{

    class TransferCommandRecorder : public CommandRecorder_I
    {
    public:
        TransferCommandRecorder(const RegistryManager& registryManager);

        void record(
            const VkCommandBuffer commandBuffer,
            const transfer::DataTransferType type,
            const uint64_t src,
            const uint64_t dest,
            const std::variant<transfer::BufferRegion, transfer::ImageRegion, transfer::BufferImageRegion>& region) const;

    private:
        const RegistryManager& mRegistryManager;
    };

}
