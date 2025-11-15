#pragma once

#include "Buffer.h"
#include "../../Types.h"

#include <concepts>
#include <cstdint>
#include <memory>

#include <vulkan/vulkan.h>

namespace ascen
{

	class BufferFactory
	{
	public:
		BufferFactory(
			VkPhysicalDevice physicalDevice,
			VkDevice device);

		template<std::derived_from<Vertex_I> T>
		BufferPtr createVertex(
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<T>& vertices) const
		{
			BufferPtr ptr = std::make_shared<Buffer>(
				Buffer::createVertexBuffer<T>(
					mPhysicalDevice,
					mDevice,
					queueFamilyIndex,
					commandPool,
					vertices));
			return ptr;
		}

		BufferPtr createVertex(
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<float>& vertices) const;

		template<typename T>
		BufferPtr createIndex(
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<T>& indices) const
		{
			BufferPtr ptr = std::make_shared<Buffer>(
				Buffer::createIndexBuffer<T>(
					mPhysicalDevice,
					mDevice,
					queueFamilyIndex,
					commandPool,
					indices));
			return ptr;
		}

		template<typename T>
		BufferPtr createUniform(uint64_t size) const
		{
			BufferPtr ptr = std::make_shared<Buffer>(
				Buffer::createUniformBuffer<T>(
					mPhysicalDevice,
					mDevice,
					size));
			return ptr;
		}

		template<typename T>
		BufferPtr createStorage(
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<T>& storage) const
		{
			BufferPtr ptr = std::make_shared<Buffer>(
				Buffer::createStorageBuffer<T>(
					mPhysicalDevice,
					mDevice,
					queueFamilyIndex,
					commandPool,
					storage));
			return ptr;
		}

		BufferPtr createIndirect(
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<Buffer::DrawCommand>& drawCommands) const;

	private:
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
