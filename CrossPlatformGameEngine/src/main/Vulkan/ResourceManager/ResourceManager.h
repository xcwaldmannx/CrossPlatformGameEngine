#pragma once

#include <vulkan/vulkan.h>

#include "MemoryAllocator/MemoryAllocator.h"
#include "Buffer/Buffer.h"

#include "../Command/CommandManager.h"
#include "../QueueFamily/QueueFamilyManager.h"

#include <stdexcept>
#include <vector>

namespace vulkan {

	class ResourceManager {
	public:
		ResourceManager();
		ResourceManager(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice);
		void init(
            const VkPhysicalDevice& physicalDevice,
            const VkDevice& logicalDevice,
            CommandManager* commandManager,
            QueueFamilyManager* queueFamilyManager
        );

        template<typename T>
        void createVertexBuffer(Buffer& buffer, std::vector<T> vertices) {

            buffer.mSize = vertices.size();
            size_t bufferSizeBytes = sizeof(vertices[0]) * vertices.size();

            Buffer stagingBuffer;

            createBuffer(
                bufferSizeBytes,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer
            );

            void* data;
            vkMapMemory(mLogicalDevice, stagingBuffer.mMemory, 0, bufferSizeBytes, 0, &data);
            std::memcpy(data, vertices.data(), bufferSizeBytes);
            vkUnmapMemory(mLogicalDevice, stagingBuffer.mMemory);

            createBuffer(
                bufferSizeBytes,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                buffer
            );

            copyBuffer(stagingBuffer, buffer, bufferSizeBytes);

            destroyBuffer(stagingBuffer);
        }

        template<typename T>
        void createIndexBuffer(Buffer& buffer, std::vector<T> indices) {

            buffer.mSize = indices.size();
            size_t bufferSizeBytes = sizeof(indices[0]) * indices.size();

            Buffer stagingBuffer;

            createBuffer(
                bufferSizeBytes,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer
            );

            void* data;
            vkMapMemory(mLogicalDevice, stagingBuffer.mMemory, 0, bufferSizeBytes, 0, &data);
            std::memcpy(data, indices.data(), bufferSizeBytes);
            vkUnmapMemory(mLogicalDevice, stagingBuffer.mMemory);

            createBuffer(
                bufferSizeBytes,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                buffer
            );

            copyBuffer(stagingBuffer, buffer, bufferSizeBytes);

            destroyBuffer(stagingBuffer);
        }

        template<typename T>
        void createUniformBuffer(Buffer& buffer, T uniform) {

            buffer.mSize = 1;
            size_t bufferSizeBytes = sizeof(T);

            Buffer stagingBuffer;

            createBuffer(
                bufferSizeBytes,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer
            );

            void* data;
            vkMapMemory(mLogicalDevice, stagingBuffer.mMemory, 0, bufferSizeBytes, 0, &data);
            std::memcpy(data, uniform, bufferSizeBytes);
            vkUnmapMemory(mLogicalDevice, stagingBuffer.mMemory);

            createBuffer(
                bufferSizeBytes,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                buffer
            );

            copyBuffer(stagingBuffer, buffer, bufferSizeBytes);

            destroyBuffer(stagingBuffer);
        }

		void destroyBuffer(Buffer& buffer);

    private:
        void createBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            Buffer& buffer
        );

        void copyBuffer(Buffer& srcBuffer, Buffer& dstBuffer, VkDeviceSize size);

	private:
		VkPhysicalDevice mPhysicalDevice = nullptr;
		VkDevice mLogicalDevice = nullptr;

		MemoryAllocator mMemoryAllocator;
        CommandManager* mCommandManager = nullptr;
        QueueFamilyManager* mQueueFamilyManager = nullptr;
	};

}