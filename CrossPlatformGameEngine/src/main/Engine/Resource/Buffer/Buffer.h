#pragma once

#include "Buffer_I.h"

#include "../../CommandPool/CommandPool.h"

namespace ascen
{

	class Buffer : public Buffer_I
	{
	public:
		struct Memory
		{
			VkDeviceSize mSize;
			uint32_t mTypeIndex;
		};

		Buffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint64_t itemCount,
			uint64_t itemSize,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryFlags);

		void destroy(VkDevice device) override;

		void upload(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkQueue queue,
			const CommandPoolPtr& commandPool,
			const void* items,
			uint32_t itemCount,
			uint32_t itemSize,
			uint32_t offset = 0) const;

		void download(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkQueue queue,
			const CommandPoolPtr& commandPool,
			void* items,
			uint32_t itemCount,
			uint32_t itemSize) const;

		size_t getItemCount() const;

		size_t getItemSize() const;

	private:
		Memory getMemoryInfo(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkBuffer buffer) const;

		size_t mItemCount = 0;      // number of items
		size_t mItemSize = 0;       // byte size of item
		VkBufferUsageFlags mUsageFlags;
		VkMemoryPropertyFlags mMemoryFlags;
	};

}