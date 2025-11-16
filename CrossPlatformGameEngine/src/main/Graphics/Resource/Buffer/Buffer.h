#pragma once

#include "../../Vertex/Vertex_I.h"
#include "../../QueueFamilies/QueueFamilies.h"
#include "../../CommandPool/CommandPool.h"

#include <iostream>
#include <concepts>
#include <memory>

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{
	struct BufferMemory
	{
		VkDeviceSize mSize;
		uint32_t mTypeIndex;
	};

	class Buffer
	{
	public:
		using DrawCommand = VkDrawIndexedIndirectCommand;

		template<std::derived_from<Vertex_I> T>
		static Buffer createVertexBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<T>& vertices)
		{
			size_t itemCount = vertices.size();
			size_t itemSize = sizeof(T);
			size_t bufferSizeBytes = itemCount * itemSize;

			Buffer stagingBuffer(
				physicalDevice,
				device,
				itemCount,
				itemSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* data;
			vkMapMemory(device, stagingBuffer.mMemory, 0, bufferSizeBytes, 0, &data);
			std::memcpy(data, vertices.data(), bufferSizeBytes);
			vkUnmapMemory(device, stagingBuffer.mMemory);

			Buffer buffer(
				physicalDevice,
				device,
				itemCount,
				itemSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VkQueue queue = QueueFamilies::getDeviceQueue(device, queueFamilyIndex);

			copy(device, queue, commandPool, stagingBuffer, buffer, false);

			destroy(device, stagingBuffer);

			return buffer;
		}

		static Buffer createVertexBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<float>& vertices)
		{
			size_t itemCount = vertices.size();
			size_t itemSize = sizeof(float);
			size_t bufferSizeBytes = itemCount * itemSize;

			Buffer stagingBuffer(
				physicalDevice,
				device,
				itemCount,
				itemSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* data;
			vkMapMemory(device, stagingBuffer.mMemory, 0, bufferSizeBytes, 0, &data);
			std::memcpy(data, vertices.data(), bufferSizeBytes);
			vkUnmapMemory(device, stagingBuffer.mMemory);

			Buffer buffer(
				physicalDevice,
				device,
				itemCount,
				itemSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VkQueue queue = QueueFamilies::getDeviceQueue(device, queueFamilyIndex);

			copy(device, queue, commandPool, stagingBuffer, buffer, false);

			destroy(device, stagingBuffer);

			return buffer;
		}

		template<typename T>
		static Buffer createIndexBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<T>& indices)
		{
			size_t itemCount = indices.size();
			size_t itemSize = sizeof(T);
			size_t bufferSizeBytes = itemCount * itemSize;

			Buffer stagingBuffer(
				physicalDevice,
				device,
				itemCount,
				itemSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* data;
			vkMapMemory(device, stagingBuffer.mMemory, 0, bufferSizeBytes, 0, &data);
			std::memcpy(data, indices.data(), bufferSizeBytes);
			vkUnmapMemory(device, stagingBuffer.mMemory);

			Buffer buffer(
				physicalDevice,
				device,
				itemCount,
				itemSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VkQueue queue = QueueFamilies::getDeviceQueue(device, queueFamilyIndex);

			copy(device, queue, commandPool, stagingBuffer, buffer, false);

			destroy(device, stagingBuffer);

			return buffer;
		}

		template<typename T>
		static Buffer createUniformBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint64_t size)
		{
			uint64_t itemCount = size;
			uint64_t itemSize = sizeof(T);
			VkDeviceSize bufferSizeBytes = static_cast<VkDeviceSize>(itemCount * itemSize);

			Buffer buffer(
				physicalDevice,
				device,
				itemCount,
				itemSize,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			vkMapMemory(device, buffer.mMemory, 0, bufferSizeBytes, 0, &buffer.mMappedMemory);

			return buffer;
		}

		template<typename T>
		static Buffer createStorageBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<T>& storage)
		{
			uint64_t itemCount = storage.size();
			uint64_t itemSize = sizeof(T);
			VkDeviceSize bufferSizeBytes = static_cast<VkDeviceSize>(itemCount * itemSize);

			Buffer stagingBuffer(
				physicalDevice,
				device,
				itemCount,
				itemSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* data;
			vkMapMemory(device, stagingBuffer.mMemory, 0, bufferSizeBytes, 0, &data);
			std::memcpy(data, storage.data(), bufferSizeBytes);
			vkUnmapMemory(device, stagingBuffer.mMemory);

			Buffer buffer(
				physicalDevice,
				device,
				itemCount,
				itemSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VkQueue queue = QueueFamilies::getDeviceQueue(device, queueFamilyIndex);

			copy(device, queue, commandPool, stagingBuffer, buffer, false);

			destroy(device, stagingBuffer);

			return buffer;
		}

		static Buffer createIndirectBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t queueFamilyIndex,
			const std::shared_ptr<CommandPool>& commandPool,
			const std::vector<DrawCommand>& storage)
		{
			uint64_t itemCount = storage.size();
			uint64_t itemSize = sizeof(DrawCommand);
			uint64_t bufferSizeBytes = itemCount * itemSize;

			Buffer stagingBuffer(
				physicalDevice,
				device,
				itemCount,
				itemSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* data = nullptr;
			vkMapMemory(device, stagingBuffer.mMemory, 0, bufferSizeBytes, 0, &data);
			std::memcpy(data, storage.data(), bufferSizeBytes);
			vkUnmapMemory(device, stagingBuffer.mMemory);

			Buffer buffer(
				physicalDevice,
				device,
				itemCount,
				itemSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT |
				VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VkQueue queue = QueueFamilies::getDeviceQueue(device, queueFamilyIndex);

			copy(device, queue, commandPool, stagingBuffer, buffer, false);

			destroy(device, stagingBuffer);

			return buffer;
		}


		static void destroy(VkDevice device, Buffer& buffer);

		static void copy(
			VkDevice device,
			VkQueue queue,
			const CommandPoolPtr& commandPool,
			Buffer& src,
			Buffer& dest,
			bool insertBarrier);

		static BufferMemory getMemoryInfo(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkBuffer buffer,
			VkMemoryPropertyFlags memoryFlags);

		VkBuffer getBuffer() const;
		VkDeviceMemory getMemory() const;
		void* getMappedMemory() const;

	private:
		Buffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint64_t itemCount,
			uint64_t itemSize,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryFlags);

	private:
		VkBuffer mBuffer;
		VkDeviceMemory mMemory;
		void* mMappedMemory;
		size_t mItemCount;      // number of items
		size_t mItemSize;       // byte size of item

	public:
		friend class Image;
	};
}