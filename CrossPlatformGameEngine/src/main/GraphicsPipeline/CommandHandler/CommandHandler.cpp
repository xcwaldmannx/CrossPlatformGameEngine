#include "CommandHandler.h"

#include "../QueueFamilyHandler/QueueFamilyHandler.h"

#include <stdexcept>

/*
Begin CommandHandler Implementation
*/

namespace ascen {

	void createCommandPool(LogicalDevice& logicalDevice, uint32_t graphicsFamily, CommandPool* commandpool) {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = graphicsFamily;

        if (vkCreateCommandPool(logicalDevice.mDevice, &poolInfo, nullptr, &commandpool->mCommandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }

        commandpool->mCommandBuffers.resize(2);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandpool->mCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandpool->mCommandBuffers.size();

        if (vkAllocateCommandBuffers(logicalDevice.mDevice, &allocInfo, commandpool->mCommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
	}

    void destroyCommandPool(LogicalDevice& logicalDevice, CommandPool& commandpool) {
        vkDestroyCommandPool(logicalDevice.mDevice, commandpool.mCommandPool, nullptr);
    }

    void recordCommandPool(
        DrawInfo& drawInfo,
        uint32_t frameIndex,
        uint32_t imageIndex,
        Swapchain& swapchain,
        RenderPass& renderpass,
        Pipeline& pipeline,
        CommandPool& commandpool) {

        vkResetCommandBuffer(commandpool.mCommandBuffers[frameIndex], 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(commandpool.mCommandBuffers[frameIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderpass.mRenderPass;
        renderPassInfo.framebuffer = swapchain.mFrameBuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapchain.mExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandpool.mCommandBuffers[frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandpool.mCommandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.mPipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchain.mExtent.width);
        viewport.height = static_cast<float>(swapchain.mExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandpool.mCommandBuffers[frameIndex], 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapchain.mExtent;
        vkCmdSetScissor(commandpool.mCommandBuffers[frameIndex], 0, 1, &scissor);

        std::array<uint32_t, 2> dynamicOffsets = {
            frameIndex * 64 * 3,  // For binding = 0, uniform buffer
            0,  // For binding = 1, storage buffer
        };

        vkCmdBindDescriptorSets(
         commandpool.mCommandBuffers[frameIndex],
         VK_PIPELINE_BIND_POINT_GRAPHICS,
         pipeline.mLayout,
         0,
         1,
         &drawInfo.mDescriptorGroup->mSets[0],
         dynamicOffsets.size(),
         dynamicOffsets.data());

        VkBuffer vertexBuffers[] = { drawInfo.mVertexBuffer->mBuffer };
        VkDeviceSize vertexOffsets[] = { 0 };
        vkCmdBindVertexBuffers(
            commandpool.mCommandBuffers[frameIndex],
            0,
            1,
            vertexBuffers,
            vertexOffsets);

        vkCmdBindIndexBuffer(
            commandpool.mCommandBuffers[frameIndex],
            drawInfo.mIndexBuffer->mBuffer,
            0,
            VK_INDEX_TYPE_UINT32);

        for (size_t i = 0; i < (*drawInfo.mEntities).size(); i++)
        {
            uint32_t vertexOffset = static_cast<uint32_t>((*drawInfo.mEntities)[i].mModel.mVertexOffset);
            uint32_t indexOffset = static_cast<uint32_t>((*drawInfo.mEntities)[i].mModel.mIndexOffset);
            uint32_t indexCount = static_cast<uint32_t>((*drawInfo.mEntities)[i].mModel.mIndexCount);

            vkCmdDrawIndexed(
                commandpool.mCommandBuffers[frameIndex],
                indexCount,
                400,
                indexOffset,
                0,
                0
            );
        }

        vkCmdEndRenderPass(commandpool.mCommandBuffers[frameIndex]);

        if (vkEndCommandBuffer(commandpool.mCommandBuffers[frameIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void beginSingleTimeCommands(
        LogicalDevice& logicalDevice,
        CommandPool& commandPool,
        VkCommandBuffer* buffer) {

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool.mCommandPool;
        allocInfo.commandBufferCount = 1;

        vkAllocateCommandBuffers(logicalDevice.mDevice, &allocInfo, buffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(*buffer, &beginInfo);
    }

    void endSingleTimeCommands(
        LogicalDevice& logicalDevice,
        VkQueue graphicsQueue,
        CommandPool& commandpool,
        VkCommandBuffer* buffer) {

        vkEndCommandBuffer(*buffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(logicalDevice.mDevice, commandpool.mCommandPool, 1, buffer);
    }
}

/*
End CommandHandler Implementation
*/