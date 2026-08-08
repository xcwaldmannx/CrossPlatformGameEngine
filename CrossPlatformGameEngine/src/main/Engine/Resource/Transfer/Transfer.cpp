#include "Transfer.h"

using namespace ascen;

void Transfer::copyBufferToBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer src,
    VkBuffer dest,
    const VkBufferCopy& region)
{
    vkCmdCopyBuffer(commandBuffer, src, dest, 1, &region);
}

void Transfer::copyBufferToImage(
    VkCommandBuffer commandBuffer,
    VkBuffer src,
    VkImage dest,
    const VkBufferImageCopy& region)
{
    vkCmdCopyBufferToImage(commandBuffer, src, dest, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void Transfer::copyImageToImage(
    VkCommandBuffer commandBuffer,
    VkImage src,
    VkImage dest,
    const VkImageCopy& region)
{
    vkCmdCopyImage(commandBuffer, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dest, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void Transfer::copyImageToBuffer(
    VkCommandBuffer commandBuffer,
    VkImage src,
    VkBuffer dest,
    const VkBufferImageCopy& region)
{
    vkCmdCopyImageToBuffer(commandBuffer, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dest, 1, &region);
}
