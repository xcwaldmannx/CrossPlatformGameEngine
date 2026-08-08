#include "TransferCommandRecorder.h"

#include "../../Resource/Transfer/Transfer.h"

using namespace ascen;

TransferCommandRecorder::TransferCommandRecorder(const RegistryManager& registryManager) :
    mRegistryManager(registryManager) {}

void TransferCommandRecorder::record(
            const VkCommandBuffer commandBuffer,
            const transfer::DataTransferType type,
            const uint64_t src,
            const uint64_t dest,
            const std::variant<transfer::BufferRegion, transfer::ImageRegion, transfer::BufferImageRegion>& region) const
{
    switch (type)
    {
        case transfer::TRANSFER_TYPE_BUFFER_TO_BUFFER:
        {
            const auto& srcResource = mRegistryManager.getResource<Buffer>(src);
            const auto& destResource = mRegistryManager.getResource<Buffer>(dest);
            VkBufferCopy copyRegion{};
            const auto& bufferRegion = std::get<transfer::BufferRegion>(region);
            copyRegion.srcOffset = bufferRegion.mSrcOffset;
            copyRegion.dstOffset = bufferRegion.mDestOffset;
            copyRegion.size      = bufferRegion.mSize;
            Transfer::copyBufferToBuffer(commandBuffer, srcResource->handle(), destResource->handle(), copyRegion);
            break;
        }
        case transfer::TRANSFER_TYPE_BUFFER_TO_IMAGE:
        {
            const auto& srcResource = mRegistryManager.getResource<Buffer>(src);
            const auto& destResource = mRegistryManager.getResource<Image>(dest);
            VkBufferImageCopy copyRegion{};
            const auto& bufferImageRegion = std::get<transfer::BufferImageRegion>(region);
            copyRegion.bufferOffset = bufferImageRegion.mBufferOffset;
            copyRegion.imageOffset = bufferImageRegion.mImageOffset;
            copyRegion.imageExtent = bufferImageRegion.mImageExtent;
            Transfer::copyBufferToImage(commandBuffer, srcResource->handle(), destResource->handle(), copyRegion);
            break;
        }
        case transfer::TRANSFER_TYPE_IMAGE_TO_IMAGE:
        {
            break;
        }
        case transfer::TRANSFER_TYPE_IMAGE_TO_BUFFER:
        {
            break;
        }
        default:
            break;
    }
}
