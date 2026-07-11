#include "CommandPool.h"

#include "../RenderPass/RenderPass.h"
#include "../Swapchain/Swapchain.h"
#include "../FrameGraph/FrameGraph.h"

#include <stdexcept>

#include <array>

using namespace ascen;

CommandPool::CommandPool(
    VkDevice device,
    uint32_t queueFamilyIndex)
{
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = queueFamilyIndex;

    mCommandBuffers.resize(2);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) mCommandBuffers.size();

    if (vkCreateCommandPool(device, &createInfo, nullptr, &mHandle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

    allocInfo.commandPool = mHandle;

    if (vkAllocateCommandBuffers(device, &allocInfo, mCommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void CommandPool::destroy(VkDevice device)
{
    vkDestroyCommandPool(device, mHandle, nullptr);
}

VkCommandBuffer CommandPool::beginCommand(uint32_t frameIndex)
{
    VkCommandBuffer commandBuffer = mCommandBuffers[frameIndex];

    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return commandBuffer;
}

void CommandPool::endCommand(VkCommandBuffer buffer)
{
    if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void CommandPool::beginRenderPass(
    const VkCommandBuffer commandBuffer,
    const RenderPassPtr& renderPass,
    const VkFramebuffer frameBuffer,
    const VkExtent2D extent)
{
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->handle();
    renderPassInfo.framebuffer = frameBuffer;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = extent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { { 0.075f, 0.01f, 0.01f, 1.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandPool::endRenderPass(VkCommandBuffer commandBuffer)
{
    vkCmdEndRenderPass(commandBuffer);
}

/*
void CommandPool::recordGraphics(
    VkCommandBuffer commandBuffer,
    const GraphicsGpuFramePass* framePass,
    uint32_t frameIndex,
    VkBuffer indirectBuffer,
    uint32_t drawCommandCount)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, framePass->mPipeline);
        
    std::vector<VkDeviceSize> vertexOffsets(framePass->mVertexBuffers.size(), 0);
    vkCmdBindVertexBuffers(
        commandBuffer,
        0,
        framePass->mVertexBuffers.size(),
        framePass->mVertexBuffers.data(),
        vertexOffsets.data());

    vkCmdBindIndexBuffer(
        commandBuffer,
        framePass->mIndexBuffer,
        0,
        VK_INDEX_TYPE_UINT32);

    // TODO: create dynamic offsets for dynamic buffers. This is currently hard-coded
    std::vector<uint32_t> dynamicOffets =
    {
        frameIndex * 64 * 2 // camera UBO, 2 mat4s, 64 bytes each
    };

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        framePass->mPipelineLayout,
        0,
        framePass->mDescriptorSets.size(),
        framePass->mDescriptorSets.data(),
        static_cast<uint32_t>(dynamicOffets.size()),
        &dynamicOffets[0]);

    vkCmdDrawIndexedIndirect(
        commandBuffer,
        indirectBuffer,
        0,
        drawCommandCount,
        sizeof(VkDrawIndexedIndirectCommand));
}

void CommandPool::recordCompute(
    VkCommandBuffer commandBuffer,
    const ComputeGpuFramePass* framePass,
    uint32_t currentFrame)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, framePass->mPipeline);

    // TODO: create dynamic offsets for dynamic buffers. This is currently hard-coded
    std::vector<uint32_t> dynamicOffets =
    {
        currentFrame * 64 * 2 // camera UBO, 2 mat4s, 64 bytes each
    };

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        framePass->mPipelineLayout,
        0,
        framePass->mDescriptorSets.size(),
        framePass->mDescriptorSets.data(),
        static_cast<uint32_t>(dynamicOffets.size()),
        &dynamicOffets[0]);

    vkCmdDispatch(commandBuffer,
        framePass->mGroups[0], framePass->mGroups[1], framePass->mGroups[2]);
}
*/

VkCommandBuffer CommandPool::beginSingle(VkDevice device)
{
    VkCommandBuffer buffer;

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mHandle;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device, &allocInfo, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffer");
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(buffer, &beginInfo);

    return buffer;
}

void CommandPool::endSingle(VkDevice device, VkQueue queue, VkCommandBuffer buffer)
{
    vkEndCommandBuffer(buffer);

    VkFence fence;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    vkCreateFence(device, &fenceInfo, nullptr, &fence);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer;

    vkQueueSubmit(queue, 1, &submitInfo, fence);
    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkDestroyFence(device, fence, nullptr);

    vkFreeCommandBuffers(device, mHandle, 1, &buffer);
}

const VkCommandBuffer* CommandPool::getBufferIndex(size_t index) const
{
    return &mCommandBuffers[index];
}
