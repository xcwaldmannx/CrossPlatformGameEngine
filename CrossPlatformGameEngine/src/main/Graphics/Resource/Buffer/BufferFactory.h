#pragma once

#include "Buffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "StorageBuffer.h"
#include "IndirectBuffer.h"
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

		//template<std::derived_from<Vertex_I> T>
		//BufferPtr createVertex(
		//	uint32_t queueFamilyIndex,
		//	const CommandPoolPtr& commandPool,
		//	const std::vector<T>& vertices) const
		//{
		//	BufferPtr ptr = std::make_shared<VertexBuffer>(
		//			mPhysicalDevice,
		//			mDevice,
		//			queueFamilyIndex,
		//			commandPool,
		//			vertices);
		//	return ptr;
		//}

		BufferPtr createVertex(
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<float>& vertices) const;

		BufferPtr createIndex(
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<uint32_t>& indices) const;

		template<typename T>
		BufferPtr createUniform(uint64_t size) const
		{
			return std::make_shared<UniformBuffer<T>>(
					mPhysicalDevice,
					mDevice,
					size);
		}

		template<typename T>
		BufferPtr createStorage(
			uint32_t queueFamilyIndex,
			const CommandPoolPtr& commandPool,
			const std::vector<T>& storage) const
		{
			return std::make_shared<StorageBuffer<T>>(
				mPhysicalDevice,
				mDevice,
				queueFamilyIndex,
				commandPool,
				storage);
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
