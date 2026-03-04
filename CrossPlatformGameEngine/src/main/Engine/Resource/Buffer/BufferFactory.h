#pragma once

#include "../../Core/Types.h"

#include <concepts>
#include <cstdint>

#include <vulkan/vulkan.h>

namespace ascen
{

	class BufferFactory
	{
	public:
		BufferFactory(
			VkPhysicalDevice physicalDevice,
			VkDevice device);

		BufferPtr createVertex(
			const CommandPoolPtr& commandPool,
			uint32_t itemCount,
			uint32_t itemSize) const;

		BufferPtr createIndex(
			const CommandPoolPtr& commandPool,
			uint32_t itemCount,
			uint32_t itemSize) const;


		BufferPtr createUniform(
			uint32_t itemCount,
			uint32_t itemSize) const;

		BufferPtr createStorage(
			const CommandPoolPtr& commandPool,
			uint32_t itemCount,
			uint32_t itemSize) const;

		BufferPtr createIndirect(
			const CommandPoolPtr& commandPool,
			uint32_t itemCount,
			uint32_t itemSize) const;

	private:
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
