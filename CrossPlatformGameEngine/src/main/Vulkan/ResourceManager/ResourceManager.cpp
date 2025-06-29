#include "ResourceManager.h"

using namespace vulkan;

ResourceManager::ResourceManager() {}

ResourceManager::ResourceManager(
	const VkPhysicalDevice& physicalDevice,
	const VkDevice& logicalDevice)
	: mPhysicalDevice(physicalDevice),
	mLogicalDevice(logicalDevice),
	mMemoryAllocator(physicalDevice, logicalDevice) {}

void ResourceManager::init(
    const VkPhysicalDevice& physicalDevice,
    const VkDevice& logicalDevice,
    CommandManager* commandManager,
    QueueFamilyManager* queueFamilyManager
) {
    mPhysicalDevice = physicalDevice;
    mLogicalDevice = logicalDevice;

    mMemoryAllocator.init(mPhysicalDevice, mLogicalDevice);

    mCommandManager = commandManager;
    mQueueFamilyManager = queueFamilyManager;
}

void ResourceManager::destroyBuffer(Buffer& buffer) {
    vkDestroyBuffer(mLogicalDevice, buffer.mBuffer, nullptr);
    vkFreeMemory(mLogicalDevice, buffer.mMemory, nullptr);
}

void ResourceManager::createBuffer(
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	Buffer& buffer
) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(mLogicalDevice, &bufferInfo, nullptr, &buffer.mBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    mMemoryAllocator.allocate(buffer, properties);
}

void ResourceManager::copyBuffer(Buffer &srcBuffer, Buffer& dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mCommandManager->getCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(mLogicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer.mBuffer, dstBuffer.mBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    uint32_t graphicsFamilyIndex = mQueueFamilyManager->getGraphicsFamilyIndex();
    const auto& graphicsQueue = mQueueFamilyManager->getDeviceQueue(mLogicalDevice, graphicsFamilyIndex);

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(mLogicalDevice, mCommandManager->getCommandPool(), 1, &commandBuffer);
}
