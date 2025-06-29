#include "CommandManager.h"

#include <stdexcept>

using namespace vulkan;

VkResult CommandManager::create(const VkDevice& logicalDevice, const QueueFamilyManager& queueFamilyManager) {
    // CommandPool

    uint32_t graphicsFamilyIndex = queueFamilyManager.getGraphicsFamilyIndex();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphicsFamilyIndex;

    if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &mVkCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }

    // CommandBuffer
    mCommandBuffers.resize(2);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = mVkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = mCommandBuffers.size();

    if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, mCommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void CommandManager::destroy(const VkDevice& logicalDevice) {
	vkDestroyCommandPool(logicalDevice, mVkCommandPool, nullptr);
}

void CommandManager::addMesh(const Mesh& mesh) {
    mMeshes.push_back(mesh);
}

void CommandManager::record(
    uint32_t frameIndex,
    uint32_t imageIndex,
    const RenderPassManager& renderpass,
    const SwapchainManager& swapchain,
    const GraphicsPipelineManager& graphicsPipeline
) {
    vkResetCommandBuffer(mCommandBuffers[frameIndex], 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(mCommandBuffers[frameIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderpass.getRenderPass();
    renderPassInfo.framebuffer = swapchain.getFrameBuffer(imageIndex);
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchain.getExtent();
    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(mCommandBuffers[frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(mCommandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getGraphicsPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchain.getExtent().width);
    viewport.height = static_cast<float>(swapchain.getExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(mCommandBuffers[frameIndex], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchain.getExtent();
    vkCmdSetScissor(mCommandBuffers[frameIndex], 0, 1, &scissor);

    // draw meshes

    for (int i = 0; i < mMeshes.size(); i++) {
        const Buffer& vertex = mMeshes[i].getVertexBuffer();
        const Buffer& index = mMeshes[i].getIndexBuffer();

        auto vertexCount = vertex.size();
        auto indexCount = index.size();

        VkDeviceSize offsets[] = { 0 };
        std::vector<VkBuffer> vbuffs = { vertex.getBuffer() };

        vkCmdBindVertexBuffers(mCommandBuffers[frameIndex], 0, 1, vbuffs.data(), offsets);
        vkCmdBindIndexBuffer(mCommandBuffers[frameIndex], index.getBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(mCommandBuffers[frameIndex], indexCount, 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(mCommandBuffers[frameIndex]);

    if (vkEndCommandBuffer(mCommandBuffers[frameIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

const VkCommandPool& CommandManager::getCommandPool() const {
    return mVkCommandPool;
}

const VkCommandBuffer& CommandManager::getCommandBuffer(size_t index) const {
    return mCommandBuffers[index];
}
