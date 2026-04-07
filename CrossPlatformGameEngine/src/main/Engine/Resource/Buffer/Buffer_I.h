#pragma once

#include "../../Handle/Handle.h"

#include <vulkan/vulkan.h>

namespace ascen
{

	typedef enum BufferUsageFlagBits
	{
		BUFFER_USAGE_VERTEX       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		BUFFER_USAGE_INDEX        = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		BUFFER_USAGE_UNIFORM      = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		BUFFER_USAGE_STORAGE      = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		BUFFER_USAGE_INDIRECT     = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
		BUFFER_USAGE_TRANSFER_SRC = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		BUFFER_USAGE_TRANSFER_DST = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	} BufferUsageFlagBits;

	typedef uint32_t BufferUsageFlags;

	typedef enum BufferMemoryFlagBits
	{
		BUFFER_MEMORY_HOST = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		BUFFER_MEMORY_LOCAL = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	} BufferMemoryFlagBits;

	typedef uint32_t BufferMemoryFlags;

	using IndexedIndirectDraw = VkDrawIndexedIndirectCommand;
	using IndirectDraw = VkDrawIndirectCommand;

	class Buffer_I : public Handle<VkBuffer>
	{
	public:
		virtual void destroy(VkDevice device) override = 0;

		VkDeviceMemory getMemory() const
		{
			return mMemory;
		}

		void* getMappedMemory() const
		{
			return mMappedMemory;
		}

	protected:
		VkDeviceMemory mMemory = VK_NULL_HANDLE;
		void* mMappedMemory = nullptr;
	};

}