#include "CommandPool.h"

#include <stdexcept>

#include <array>

using namespace ascen;

CommandPool::CommandPool(uint32_t graphicsFamily)
{
    mCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    mCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    mCreateInfo.queueFamilyIndex = graphicsFamily;

    mCommandBuffers.resize(2);

    VkCommandBufferAllocateInfo allocInfo{};
    mAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    mAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    mAllocInfo.commandBufferCount = (uint32_t) mCommandBuffers.size();
}

void CommandPool::create(VkDevice device)
{
    if (vkCreateCommandPool(device, &mCreateInfo, nullptr, &mHandle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

    mAllocInfo.commandPool = mHandle;

    if (vkAllocateCommandBuffers(device, &mAllocInfo, mCommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void CommandPool::destroy(VkDevice device) {
    vkDestroyCommandPool(device, mHandle, nullptr);
}

// TODO: create a replacement for DrawInfo that gets passed into record for rendering entities

void CommandPool::record(
    uint32_t frameIndex,
    uint32_t imageIndex,
    VkDescriptorSet descriptorSet,
    VkBuffer vertexBuffer,
    VkBuffer indexBuffer,
    std::shared_ptr<RenderPass> renderPass,
    std::shared_ptr<Swapchain> swapchain,
    std::shared_ptr<Pipeline> pipeline)
{
    vkResetCommandBuffer(mCommandBuffers[frameIndex], 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(mCommandBuffers[frameIndex], &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    const VkExtent2D& renderArea = swapchain->getExtent();

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->handle();
    renderPassInfo.framebuffer = swapchain->getFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = renderArea;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(mCommandBuffers[frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(mCommandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->handle());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(renderArea.width);
    viewport.height = static_cast<float>(renderArea.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(mCommandBuffers[frameIndex], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = renderArea;
    vkCmdSetScissor(mCommandBuffers[frameIndex], 0, 1, &scissor);

    std::array<uint32_t, 2> dynamicOffsets =
    {
        frameIndex * 64 * 2,  // For binding = 0, uniform buffer
        0,                    // For binding = 1, storage buffer
    };

    vkCmdBindDescriptorSets(
        mCommandBuffers[frameIndex],
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline->getLayout(),
        0,
        1,
        &descriptorSet,
        dynamicOffsets.size(),
        dynamicOffsets.data());

    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize vertexOffsets[] = { 0 };
    vkCmdBindVertexBuffers(
        mCommandBuffers[frameIndex],
        0,
        1,
        vertexBuffers,
        vertexOffsets);

    vkCmdBindIndexBuffer(
        mCommandBuffers[frameIndex],
        indexBuffer,
        0,
        VK_INDEX_TYPE_UINT32);

    // TODO: get model data in here and render meshes appropriately
    int instanceOffset = 0;

    for (auto& [modelId, instanceCount] : *drawInfo.mModelIdToCount)
    {
        const Model& model = drawInfo.mModelManager->getModel(modelId);

        uint32_t vertexOffset = static_cast<uint32_t>(model.mMesh.mVertexOffset);
        uint32_t indexOffset = static_cast<uint32_t>(model.mMesh.mIndexOffset);
        uint32_t indexCount = static_cast<uint32_t>(model.mMesh.mIndexCount);

        vkCmdDrawIndexed(
            mCommandBuffers[frameIndex],
            indexCount,
            instanceCount,
            indexOffset,
            0,
            instanceOffset
        );

        instanceOffset += instanceCount;
    }

    vkCmdEndRenderPass(mCommandBuffers[frameIndex]);

    if (vkEndCommandBuffer(mCommandBuffers[frameIndex]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void CommandPool::beginSingleTimeCommands(
    VkDevice device,
    VkCommandBuffer* buffer)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mHandle;
    allocInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(device, &allocInfo, buffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(*buffer, &beginInfo);
}

void CommandPool::endSingleTimeCommands(
    VkDevice device,
    VkQueue graphicsQueue,
    VkCommandBuffer* buffer)
{
    vkEndCommandBuffer(*buffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, mHandle, 1, buffer);
}
