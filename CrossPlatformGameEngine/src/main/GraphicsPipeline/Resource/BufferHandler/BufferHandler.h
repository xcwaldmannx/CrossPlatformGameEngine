#pragma once

#include "../ResourceCommonTypes.h"
#include "../ResourceCommonFunctions.h"

namespace ascen {

    template<typename T>
	void createVertexBuffer(
		const BufferCreateInfo& info,
		Buffer& buffer,
		const std::vector<T>& vertices) {

		buffer.mSize = vertices.size();
		buffer.mSizeBytes = sizeof(vertices[0]) * vertices.size();

		Buffer stagingBuffer;

		createBuffer(
			info.mPhysicalDevice->mDevice,
			info.mLogicalDevice->mDevice,
			buffer.mSizeBytes,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer);

		void* data;
		vkMapMemory(info.mLogicalDevice->mDevice, stagingBuffer.mMemory, 0, buffer.mSizeBytes, 0, &data);
		std::memcpy(data, vertices.data(), buffer.mSizeBytes);
		vkUnmapMemory(info.mLogicalDevice->mDevice, stagingBuffer.mMemory);

		createBuffer(
			info.mPhysicalDevice->mDevice,
			info.mLogicalDevice->mDevice,
			buffer.mSizeBytes,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&buffer);

		copyBuffer(
			*info.mLogicalDevice,
			info.mGraphicsQueue,
			*info.mCommandPool,
			stagingBuffer,
			buffer,
			buffer.mSizeBytes,
			false);

		destroyBuffer(*info.mLogicalDevice, stagingBuffer);
	}

	template<typename T>
	void createIndexBuffer(
		const BufferCreateInfo& info,
		Buffer& buffer,
		const std::vector<T>& indices) {

		buffer.mSize = indices.size();
		buffer.mSizeBytes = sizeof(indices[0]) * indices.size();

		Buffer stagingBuffer;

		createBuffer(
			info.mPhysicalDevice->mDevice,
			info.mLogicalDevice->mDevice,
			buffer.mSizeBytes,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer);

		void* data;
		vkMapMemory(info.mLogicalDevice->mDevice, stagingBuffer.mMemory, 0, buffer.mSizeBytes, 0, &data);
		std::memcpy(data, indices.data(), buffer.mSizeBytes);
		vkUnmapMemory(info.mLogicalDevice->mDevice, stagingBuffer.mMemory);

		createBuffer(
			info.mPhysicalDevice->mDevice,
			info.mLogicalDevice->mDevice,
			buffer.mSizeBytes,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&buffer);

		copyBuffer(
			*info.mLogicalDevice,
			info.mGraphicsQueue,
			*info.mCommandPool,
			stagingBuffer,
			buffer,
			buffer.mSizeBytes,
			false);

		destroyBuffer(*info.mLogicalDevice, stagingBuffer);
	}

	template<typename T>
	void createUniformBuffer(
		const BufferCreateInfo& info,
		const size_t size,
		Buffer& buffer) 
	{
		buffer.mSize = size;
		buffer.mSizeBytes = sizeof(T);
		buffer.mBufferSizeBytes = sizeof(T) * size;

		createBuffer(
			info.mPhysicalDevice->mDevice,
			info.mLogicalDevice->mDevice,
			buffer.mBufferSizeBytes,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&buffer);

		vkMapMemory(info.mLogicalDevice->mDevice, buffer.mMemory, 0, buffer.mBufferSizeBytes, 0, &buffer.mMappedMemory);
	}

	template<typename T>
	void createStorageBuffer(
		const BufferCreateInfo& info,
		const std::vector<T>& data,
		Buffer& buffer)
	{

		buffer.mSize = data.size();
		buffer.mSizeBytes = sizeof(T);
		buffer.mBufferSizeBytes = sizeof(T) * data.size();

		Buffer stagingBuffer;

		createBuffer(
			info.mPhysicalDevice->mDevice,
			info.mLogicalDevice->mDevice,
			buffer.mBufferSizeBytes,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer);

		void* mem;
		vkMapMemory(info.mLogicalDevice->mDevice, stagingBuffer.mMemory, 0, buffer.mBufferSizeBytes, 0, &mem);
		std::memcpy(mem, data.data(), buffer.mBufferSizeBytes);
		vkUnmapMemory(info.mLogicalDevice->mDevice, stagingBuffer.mMemory);

		createBuffer(
			info.mPhysicalDevice->mDevice,
			info.mLogicalDevice->mDevice,
			buffer.mBufferSizeBytes,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&buffer);

		copyBuffer(
			*info.mLogicalDevice,
			info.mGraphicsQueue,
			*info.mCommandPool,
			stagingBuffer,
			buffer,
			buffer.mBufferSizeBytes,
			true);

		destroyBuffer(*info.mLogicalDevice, stagingBuffer);
	}

}