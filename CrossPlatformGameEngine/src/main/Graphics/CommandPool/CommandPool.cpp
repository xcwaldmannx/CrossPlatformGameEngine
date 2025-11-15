#include "CommandPool.h"

#include <stdexcept>

#include <array>

using namespace ascen;

CommandPool::CommandPool(uint32_t queueFamilyIndex)
{
    mCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    mCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    mCreateInfo.queueFamilyIndex = queueFamilyIndex;

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

void CommandPool::record(
    VkPhysicalDevice physicalDevice,
    uint32_t currentFrame,
    uint32_t currentImage,
    VkBuffer vertexBuffer,
    VkBuffer indexBuffer,
    VkBuffer indirectBuffer,
    const DescriptorSetPtr& descriptorSet,
    const RenderPassPtr& renderPass,
    const SwapchainPtr& swapchain,
    const GraphicsPipelinePtr& pipeline,
    const std::vector<VkDrawIndexedIndirectCommand>& drawCommands)
{
    vkResetCommandBuffer(mCommandBuffers[currentFrame], 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(mCommandBuffers[currentFrame], &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    const VkExtent2D& renderArea = swapchain->getExtent();

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->handle();
    renderPassInfo.framebuffer = swapchain->getFramebuffers()[currentImage];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = renderArea;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    auto& currentCommandBuffer = mCommandBuffers[currentFrame];

    vkCmdBeginRenderPass(currentCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->handle());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(renderArea.width);
    viewport.height = static_cast<float>(renderArea.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = renderArea;
    vkCmdSetScissor(currentCommandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize vertexOffsets[] = { 0 };
    vkCmdBindVertexBuffers(
        currentCommandBuffer,
        0,
        1,
        vertexBuffers,
        vertexOffsets);

    vkCmdBindIndexBuffer(
        currentCommandBuffer,
        indexBuffer,
        0,
        VK_INDEX_TYPE_UINT32);

    std::vector<uint32_t> dynamicOffets =
    {
        currentFrame * 64 * 2 // camera UBO, 2 mat4s, 64 bytes each
    };

    VkDescriptorSet set = descriptorSet->handle();

    vkCmdBindDescriptorSets(
        currentCommandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline->getLayout(),
        0,
        1,
        &set,
        static_cast<uint32_t>(dynamicOffets.size()),
        &dynamicOffets[0]);

    vkCmdDrawIndexedIndirect(
        currentCommandBuffer,
        indirectBuffer,
        0,
        static_cast<uint32_t>(drawCommands.size()),
        sizeof(VkDrawIndexedIndirectCommand));

    vkCmdEndRenderPass(currentCommandBuffer);

    if (vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

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

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, mHandle, 1, &buffer);
}

const VkCommandBuffer* CommandPool::getBufferIndex(size_t index) const
{
    return &mCommandBuffers[index];
}
