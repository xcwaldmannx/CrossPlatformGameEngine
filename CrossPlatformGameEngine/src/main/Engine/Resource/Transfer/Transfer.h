#pragma once

#include "../Buffer/Buffer.h"

namespace ascen
{

    class Transfer
    {
    public:
        static void copyBufferToBuffer(
            VkCommandBuffer commandBuffer,
            VkBuffer src,
            VkBuffer dest,
            const VkBufferCopy& region);

        static void copyBufferToImage(
            VkCommandBuffer commandBuffer,
            VkBuffer src,
            VkImage dest,
            const VkBufferImageCopy& region);

        static void copyImageToImage(
            VkCommandBuffer commandBuffer,
            VkImage src,
            VkImage dest,
            const VkImageCopy& region);

        static void copyImageToBuffer(
            VkCommandBuffer commandBuffer,
            VkImage src,
            VkBuffer dest,
            const VkBufferImageCopy& region);
    };

}
