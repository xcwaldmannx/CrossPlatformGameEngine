#pragma once

#include "../../Vertex/Vertex_I.h"
#include "../../CommandPool/CommandPool.h"

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
		template<std::derived_from<Vertex_I> T>
		static Buffer createVertexBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkQueue graphicsQueue,
			std::shared_ptr<CommandPool> commandPool,
			const std::vector<T>& vertices)
		{
			size_t itemCount = vertices.size();
			size_t itemSize = sizeof(vertices[0]);
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

			copy(
				device,
				graphicsQueue,
				commandPool,
				stagingBuffer,
				buffer,
				false);

			destroyBuffer(device, stagingBuffer);

			return buffer;
		}

		template<typename T>
		static Buffer createIndexBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkQueue graphicsQueue,
			std::shared_ptr<CommandPool> commandPool,
			const std::vector<T>& indices)
		{
			size_t itemCount = indices.size();
			size_t itemSize = sizeof(indices[0]);
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

			copy(
				device,
				graphicsQueue,
				commandPool,
				stagingBuffer,
				buffer,
				false);

			destroyBuffer(device, stagingBuffer);

			return buffer;
		}

		template<typename T>
		static Buffer createUniformBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkDeviceSize size)
		{
			size_t itemCount = size;
			size_t itemSize = sizeof(T);
			size_t bufferSizeBytes = itemCount * itemSize;

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
			VkQueue graphicsQueue,
			std::shared_ptr<CommandPool> commandPool,
			const std::vector<T>& storage)
		{
			size_t itemCount = storage.size();
			size_t itemSize = sizeof(storage[0]);
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
			std::memcpy(data, storage.data(), bufferSizeBytes);
			vkUnmapMemory(device, stagingBuffer.mMemory);

			Buffer buffer(
				physicalDevice,
				device,
				itemCount,
				itemSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			copy(
				device,
				graphicsQueue,
				commandPool,
				stagingBuffer,
				buffer,
				false);

			destroyBuffer(device, stagingBuffer);

			return buffer;
		}

		static void destroyBuffer(VkDevice device, Buffer& buffer);

		static void copy(
			VkDevice device,
			VkQueue queue,
			std::shared_ptr<CommandPool> commandPool,
			Buffer& src,
			Buffer& dest,
			bool insertBarrier);

		static BufferMemory getMemory(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkBuffer buffer,
			VkMemoryPropertyFlags memoryFlags);

	private:
		Buffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			size_t itemCount,
			size_t itemSize,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryFlags);

	private:
		VkBuffer mBuffer;
		VkDeviceMemory mMemory;
		void* mMappedMemory;
		size_t mItemCount;      // number of elements in the buffer
		size_t mItemSize;       // size of one element in the buffer
	};
}